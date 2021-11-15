#include "userconfig.h"

#include <stdexcept>

#include "INIReader.h"

Config::Config(const std::filesystem::path& config_path)
{
  //
  path = config_path;
  INIReader reader(config_path);
  if (reader.ParseError() < 0)
  {
    throw std::runtime_error("Unable to read config: " + config_path.string());
  }

  name = reader.Get("user", "name", "UNKNOWN");
  email = reader.Get("user", "email", "UNKNOWN");
  username = reader.Get("dotEngine", "username", "UNKNOWN");
  cache_path = reader.Get("dotEngine", "cachePath", "/tmp/dotEngineCache.json");

  // TODO: Key
  key = "";
}

std::ostream& operator<<(std::ostream& out, const Config& conf)
{
  out << "Name: " << conf.get_name() << std::endl
      << "Email: " << conf.get_email() << std::endl
      << "Username: " << conf.get_username() << std::endl
      << "Cache Path: " << conf.get_cache_path();

  return out;
}

std::string Config::get_name() const
{
  //
  return name;
}

std::string Config::get_email() const
{
  //
  return email;
}

std::string Config::get_username() const
{
  //
  return username;
}

std::string Config::get_key() const
{
  //
  return key;
}

std::filesystem::path Config::get_cache_path() const
{
  //
  return cache_path;
}
