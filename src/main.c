
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

/* Argument parsing */
const char* argp_program_version = dotEngine_VERSION;
/* static const char argp_bug_address[] = "<joshua.gf.arul@gmail.com>"; */
static const char args_doc[] = "";
static const char doc[] = dotEngine_DESCRIPTION;

static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output."},
    {"force", 'f', 0, 0, "Ignore the cache and force a GET."},
    {"output", 'o', "FILE", 0, "Output to FILE instead of stdout"},
    {0}};

struct arguments
{
  int verbose;
  int force;
  char* output_file;
};

// Parse a single option
static error_t parse_opt(int key, char* arg, struct argp_state* state);

// My arg parse
static struct argp argp = {options, parse_opt, args_doc, doc};
int main(int argc, char** argv)
{
  struct arguments arguments;
  // Default argument options
  arguments.verbose = 0;
  arguments.force = 0;
  arguments.output_file = "-";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  const char base_url[] = "https://api.github.com/users/%s/repos?per_page=250";
  char* url = NULL;
  struct repo* repos = NULL;

  // Receiver for data from GitHub API
  struct string raw_data;
  init_string_base(&raw_data);

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
  init_configuration(&config);
  if (ini_parse(full_path, handler, &config) < 0 || config.username == NULL)
  {
    fprintf(stderr, "Can't load '%s'\n", full_path);
    ret = 1;
    goto cleanup;
  }

  // Get the real url to GitHub API
  size_t sz = snprintf(NULL, 0, base_url, config.username) + 1;
  url = (char*)malloc(sz);
  if (url == NULL)
  {
    fprintf(stderr, "Couldn't allocate memory!");
    ret = 1;
    goto cleanup;
  }
  snprintf(url, sz, base_url, config.username);

#ifndef NDEBUG
  printf("Config loaded from '%s': name=%s, email=%s, username=%s\n", full_path,
         config.name, config.email, config.username);
  printf("URL: %s\n", url);
#endif

  if (!http_get(url, &raw_data))
  {
    fprintf(stderr, "Can't connect to Github API... internet connectivity?\n");
    ret = 1;
    goto cleanup;
  }

  size_t num_repos;
  if (!get_repos(&raw_data, &repos, &num_repos) || repos == NULL)
  {
    fprintf(stderr, "Couldn't parse response from GitHub API.\n");
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

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
  struct arguments* arguments = state->input;

  switch (key)
  {
    case 'v':
      arguments->verbose = 1;
      break;
    case 'f':
      arguments->force = 1;
      break;
    case 'o':
      arguments->output_file = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->argc > 0) /* Too many arguments. */
        argp_usage(state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
