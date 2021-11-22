#include "gh.h"

#include <curl/curl.h>
#include <sys/stat.h>
#include <time.h>

#include <algorithm>
#include <boost/json.hpp>
#include <boost/json/parser.hpp>
#include <fstream>
#include <iostream>

GitHub::GitHub(const Config& conf) : config(conf)
{
  std::string uname = conf.get_username();
  const size_t buf_size = uname.size() + base_url.size() + 1;
  endpoint = new char[buf_size];
  std::snprintf(endpoint, buf_size, base_url.c_str(), uname.c_str());
  get_repos(false);
}

GitHub::~GitHub()
{  //
  delete[] endpoint;
}

static size_t curl_callback(void* contents, size_t size, size_t nmemb,
                            std::string* s)

{
  size_t new_size = size * nmemb;
  s->append((char*)contents, new_size);

  return new_size;
}

void GitHub::get_repos(const bool& force)
{
  if (!force && !stale_cache() && read_cache())
  {
    return;
  }
  CURL* curl;
  CURLcode res;
  size_t http_resp;
  std::string buffer;
  buffer.reserve(4096);

  curl = curl_easy_init();
  if (!curl)
  {
    throw std::runtime_error("Failed initializing curl");
  }

  curl_easy_setopt(curl, CURLOPT_URL, endpoint);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "dotEngine/1.0");
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);

  res = curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_resp);

  if (http_resp != 200)
  {
    throw std::runtime_error("Invalid GitHub response: " +
                             std::to_string(http_resp));
  }

  write_cache(buffer);

  boost::json::parser p;
  p.write_some(buffer);
  jv = p.release();
}

void GitHub::write_cache(const std::string& buffer) const
{
  const std::filesystem::path cache_path = config.get_cache_path();
  std::ofstream out_file(cache_path);
  out_file << buffer << std::endl;
  out_file.close();
}

bool GitHub::read_cache()
{
  const std::filesystem::path cache_path = config.get_cache_path();
  std::ifstream in_file(cache_path);
  if (!in_file.is_open())
  {
    return false;
  }
  std::string buffer((std::istreambuf_iterator<char>(in_file)),
                     std::istreambuf_iterator<char>());

  boost::json::parser p;
  try
  {
    p.write_some(buffer);
    jv = p.release();
  }
  catch (...)
  {
    return false;
  }

  return true;
}

bool GitHub::stale_cache()
{
  const std::filesystem::path cache_path = config.get_cache_path();
  struct stat cache;
  struct timespec current;

  if (stat(cache_path.string().c_str(), &cache) != 0 ||
      clock_gettime(CLOCK_REALTIME, &current) < 0)
  {
    return true;
  }
  if (difftime(cache.st_mtime, current.tv_sec) > 5 * 60)
  {
    return true;
  }

  return false;
}

static std::string& clean(std::string& str)
{
  str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '\''), str.end());

  return str;
}

std::string GitHub::zsh_completion(const bool& force)
{
  std::stringstream buffer;
  std::string name;
  std::string descrip;

  if (force)
  {
    get_repos(true);
  }

  for (auto i : jv.get_array())
  {
    name = std::string(i.as_object()["name"].as_string());
    clean(name);

    if (i.as_object()["description"].is_null())
    {
      descrip = "";
    }
    else
    {
      descrip = i.as_object()["description"].as_string();
      clean(descrip);
    }

    buffer << '"' << name << "\":\"" << descrip << '"' << std::endl;
  }
  return buffer.str();
}
