#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <filesystem>
#include <string>

class Config
{
  std::filesystem::path path;
  std::string name;
  std::string email;
  std::string username;
  std::string key;
  std::filesystem::path cache_path;

  void load();

 public:
  Config(const std::filesystem::path& config_path);
  std::string get_name() const;
  std::string get_email() const;
  std::string get_username() const;
  std::string get_key() const;
  std::filesystem::path get_cache_path() const;
};

std::ostream& operator<<(std::ostream& out, const Config& conf);
#endif
