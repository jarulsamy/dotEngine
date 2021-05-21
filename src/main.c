
#include <stdio.h>
#include <string.h>

#include "http.h"
#include "ini.h"
#include "str.h"
#include "username.h"

int main()
{
  const char base_url[] = "https://api.github.com/users/%s/repos?per_page=250";

  // Get the full path of ~/.gitconfig
  const char *home = getenv("HOME");
  const char *filename = "/.gitconfig";
  size_t len = strlen(home) + strlen(filename) + 1;
  char *full_path = malloc(sizeof(char) * len);
  strcpy(full_path, home);
  strcat(full_path, filename);

  // Load username from file
  struct configuration config;
  if (ini_parse(full_path, handler, &config) < 0)
  {
    printf("Can't load '%s'\n", full_path);
    return 1;
  }
  printf("Config loaded from '%s': name=%s, email=%s\n", full_path, config.name,
         config.email);

  free(full_path);
  free_configuration(&config);

  return 0;
}
