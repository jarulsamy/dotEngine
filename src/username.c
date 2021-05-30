
#include "username.h"

#include <stdlib.h>
#include <string.h>

#include "parser.h"

void init_configuration(struct configuration* config)
{
  config->name = NULL;
  config->email = NULL;
  config->username = NULL;
}

void free_configuration(struct configuration* config)
{
  free(config->name);
  free(config->email);
  free(config->username);

  // Set all members to NULL
  init_configuration(config);
}

int handler(void* user, const char* section, const char* name,
            const char* value)
{
  struct configuration* pconfig = (struct configuration*)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("user", "name"))
  {
    pconfig->name = strdup(value);
  }
  else if (MATCH("user", "email"))
  {
    pconfig->email = strdup(value);
  }
  else if (MATCH("user", "username"))
  {
    pconfig->username = strdup(value);
  }
  // Unknown section / name, error.
  else
  {
    return 0;
  }

  return 1;
}
