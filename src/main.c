#include <argp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "http.h"
#include "ini.h"
#include "io.h"
#include "repo.h"

int github_get(struct repo** repos, size_t* num_repos);

/* Argument parsing */
const char* argp_program_version = dotEngine_VERSION;
/* static const char argp_bug_address[] = "<joshua.gf.arul@gmail.com>"; */
static const char args_doc[] = "";
static const char doc[] = dotEngine_DESCRIPTION;

static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output."},
    {"force", 'f', 0, 0, "Ignore the cache and force a GET."},
    {"zsh_completion", 'z', 0, 0, "Output ZSH autocompletion information"},
    {"output", 'o', "FILE", 0, "Output to FILE instead of stdout"},
    {0}};

struct arguments
{
  int verbose;
  int force;
  int zsh_completion;
  char* output_file;
};

// Parse a single option
static error_t parse_opt(int key, char* arg, struct argp_state* state);

// My arg parse
static struct argp argp = {options, parse_opt, args_doc, doc};

/* Constants */
const char base_url[] = "https://api.github.com/users/%s/repos?per_page=250";
const char base_url_auth[] = "https://api.github.com/user/repos?per_page=250";
const char* CACHE_FNAME = "/tmp/dotEngine.dat";

int main(int argc, char** argv)
{
  struct repo* repos = NULL;
  size_t num_repos = 0;

  struct arguments arguments;
  // Default argument options
  arguments.verbose = 0;
  arguments.force = 0;
  arguments.zsh_completion = 0;
  arguments.output_file = "-";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // Track errors - return code
  int ret = 0;

  // Try to use the cache
  if (!arguments.force && valid_cache(CACHE_FNAME))
  {
    // Read from disk
    FILE* read_fp = fopen(CACHE_FNAME, "r");
    if (read_fp == NULL || !repo_from_file(read_fp, &repos, &num_repos))
    {
      fprintf(stderr, "Couldn't read from disk.\n");
      ret = 1;
      goto cleanup;
    }
    fclose(read_fp);
  }
  // Otherwise, contact the API
  else
  {
    if (!github_get(&repos, &num_repos))
    {
      ret = 1;
      goto cleanup;
    }
  }

  // At this point, repos must have be correctly populated with github repos.
  if (arguments.zsh_completion)
  {
    repo_print_zshsuggestion(repos, num_repos);
  }

cleanup:
  if (repos != NULL)
  {
    for (size_t i = 0; i < num_repos; i++)
    {
      repo_free(&repos[i]);
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
    case 'z':
      arguments->zsh_completion = 1;
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

int github_get(struct repo** repos, size_t* num_repos)
{
  // Future dynamic allocations, forward defined for easy cleanup
  char* url = NULL;

  char* full_path = NULL;
  struct config config;
  config_init(&config);

  // Receiver for data from GitHub API
  char* raw_data = NULL;

  int ret = 1;

  // Get the full path of ~/.gitconfig
  const char* home = getenv("HOME");
  const char* filename = "/.gitconfig";
  size_t len = strlen(home) + strlen(filename) + 1;
  full_path = malloc(sizeof(char) * len);
  strcpy(full_path, home);
  strcat(full_path, filename);

  // Load username and password from file
  if (ini_parse(full_path, config_ini_handler, &config) < 0 ||
      config.username == NULL)
  {
    fprintf(stderr, "Can't load '%s'\n", full_path);
    ret = 0;
    goto cleanup;
  }

  if (config.password == NULL)
  {
    config.password = strdup(getenv("DOTENGINE_GHPWD"));
  }

  // We've checked everywhere, no password found so don't authenticate.
  if (config.password == NULL)
  {
    // Get the real url to GitHub API
    size_t sz = snprintf(NULL, 0, base_url, config.username) + 1;
    url = (char*)malloc(sz);
    snprintf(url, sz, base_url, config.username);
  }
  else
  {
    url = strdup(base_url_auth);
  }
  if (url == NULL)
  {
    fprintf(stderr, "Couldn't allocate memory!");
    ret = 0;
    goto cleanup;
  }

#ifndef NDEBUG
  fprintf(
      stderr,
      "Config loaded from '%s': name=%s, email=%s, username=%s password=%s\n",
      full_path, config.name, config.email, config.username, config.password);
  fprintf(stderr, "URL: %s\n", url);
#endif

  // Talk to the API
  long http_code = http_get(url, &raw_data, config.username, config.password);
  if (http_code == 401)
  {
    fprintf(stderr, "Unauthorized, check username and password.\n");
    ret = 1;
    goto cleanup;
  }
  else if (http_code != 200)
  {
    fprintf(stderr,
            "Can't connect to Github API... internet connectivity?, HTTP "
            "Status: %li\n",
            http_code);
    ret = 1;
    goto cleanup;
  }

  // Parse the response from Github
  if (!repo_parse(raw_data, repos, num_repos) || repos == NULL)
  {
    fprintf(stderr, "Couldn't parse response from GitHub API.\n");
    ret = 1;
    goto cleanup;
  }

  // Save to disk
  FILE* dest_fp = fopen(CACHE_FNAME, "w");
  if (dest_fp == NULL || !repo_to_file(dest_fp, *repos, *num_repos))
  {
    fprintf(stderr, "Couldn't write to disk.\n");
    ret = 1;
    goto cleanup;
  }
  fclose(dest_fp);

cleanup:
  free(full_path);
  free(url);
  config_free(&config);
  free(raw_data);

  return ret;
}
