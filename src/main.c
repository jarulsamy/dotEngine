
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "http.h"
#include "ini.h"
#include "parser.h"
#include "repo.h"
#include "username.h"

const char base_url[] = "https://api.github.com/users/%s/repos?per_page=250";

int main()
{
  char* full_path;
  char* url = NULL;
  struct repo* repos = NULL;

  // Track errors - return code
  int ret = 0;

  // Get the full path of ~/.gitconfig
  const char* home = getenv("HOME");
  const char* filename = "/.gitconfig";
  size_t len = strlen(home) + strlen(filename) + 1;
  full_path = malloc(sizeof(char) * len);
  strcpy(full_path, home);
  strcat(full_path, filename);

  // Load username from file
  struct configuration config;
  if (ini_parse(full_path, handler, &config) < 0)
  {
    printf("Can't load '%s'\n", full_path);
    ret = 1;
    goto cleanup;
  }
  printf("Config loaded from '%s': name=%s, email=%s, username=%s\n", full_path,
         config.name, config.email, config.username);

  // Get the real url to HTTP GET.
  size_t sz = snprintf(NULL, 0, base_url, config.username);
  url = (char*)malloc(sz + 1);
  if (url == NULL)
  {
    fprintf(stderr, "Couldn't allocate memory!");
    ret = 1;
    goto cleanup;
  }
  snprintf(url, sz + 1, base_url, config.username);
  printf("URL: %s\n", url);

  struct string raw_data;
  http_get(url, &raw_data);

  size_t num_repos;
  get_repos(&raw_data, &repos, &num_repos);
  if (repos == NULL)
  {
    goto cleanup;
  }

cleanup:
  free(full_path);
  free(url);
  free_configuration(&config);
  free_string(&raw_data);

  if (repos != NULL)
  {
    for (size_t i = 0; i < num_repos; i++)
    {
      free_repo(&repos[i]);
    }
    free(repos);
  }

  return ret;
}
