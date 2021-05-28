
#include <argp.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "http.h"
#include "ini.h"
#include "parser.h"
#include "repo.h"
#include "username.h"

const char* argp_program_version = dotEngine_VERSION;
const char* argp_program_bug_address = "<joshua.gf.arul@gmail.com>";

/* Argument parsing */
static char doc[] = dotEngine_DESCRIPTION;
static struct argp argp = {0, 0, 0, doc};

const char base_url[] = "https://api.github.com/users/%s/repos?per_page=250";
int main(int argc, char** argv)
{
  argp_parse(&argp, argc, argv, 0, 0, 0);

  char* url = NULL;
  struct repo* repos = NULL;

  // Track errors - return code
  int ret = 0;

  // Get the full path of ~/.gitconfig
  const char* home = getenv("HOME");
  const char* filename = "/.gitconfig";
  size_t len = strlen(home) + strlen(filename) + 1;
  char* full_path = malloc(sizeof(char) * len);
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

  // Get the real url to GitHub API
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
  if (!http_get(url, &raw_data))
  {
    fprintf(stderr, "Can't connect to Github API... internet connectivity?\n");
    ret = 1;
    goto cleanup;
  }

  size_t num_repos;
  if (!get_repos(&raw_data, &repos, &num_repos) || repos == NULL)
  {
    fprintf(stderr, "Couldn't parse response from GitHub API\n");
    ret = 1;
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
