
#include <argp.h>
#include <unistd.h>

#include <iostream>

#include "config.h"
#include "gh.h"
#include "userconfig.h"

/* Argument parsing */
const char* argp_program_version = dotEngine_VERSION;
const char* argp_program_bug_address = "<joshua.gf.arul@gmail.com>";
static char doc[] = "An overkill CLI app to contact the GitHub API.";

static struct argp_option options[] = {
    {"force", 'f', 0, 0, "Ignore the cache and force a GET."},
    {"zsh_completion", 'z', 0, 0, "Output ZSH autocompletion information"},
    {"output", 'o', "FILE", 0, "Output to FILE instead of stdout"},
    {0}};

struct arguments
{
  int force;
  int zsh_completion;
};

static error_t parse_opt(int key, char* arg, argp_state* state);
static argp ap = {options, parse_opt, doc, doc};

int main(int argc, char** argv)
{
  arguments args;
  args.force = 0;
  args.zsh_completion = 0;

  if (argp_parse(&ap, argc, argv, 0, 0, &args) != 0)
  {
    return 1;
  }

  const std::filesystem::path config_path =
      std::string(getenv("HOME")) + std::string("/.gitconfig");
  Config conf = Config(std::filesystem::path(config_path));
  GitHub gh = GitHub(conf);

  if (args.zsh_completion)
  {
    std::cout << gh.zsh_completion(args.force) << std::endl;
  }

  return 0;
}

static error_t parse_opt(int key, char* arg, argp_state* state)
{
  arguments* args = (arguments*)state->input;

  switch (key)
  {
    case 'f':
      args->force = 1;
      break;
    case 'z':
      args->zsh_completion = 1;
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
