#ifndef GH_H
#define GH_H

#include <curl/curl.h>

#include <boost/json.hpp>
#include <string>

#include "userconfig.h"

class GitHub
{
  inline static const std::string base_url =
      "https://api.github.com/users/%s/repos?per_page=250";
  inline static const std::string base_url_auth =
      "https://api.github.com/user/repos?per_page=250";

  Config config;
  char* endpoint;
  boost::json::value jv;

  void get_repos(const bool& force);
  void write_cache(const std::string& buffer) const;
  bool read_cache();

 public:
  GitHub(const Config& conf);
  ~GitHub();
  std::string zsh_completion(const bool& force);
};

#endif
