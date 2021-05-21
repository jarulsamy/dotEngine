
#include "username.h"

#include "parser.h"

/* #include <stdio.h> */
#include <stdlib.h>
#include <string.h>

void free_configuration(struct configuration* config)
{
  if (config->name != NULL) free(config->name);
  if (config->email != NULL) free(config->email);
  if (config->username != NULL) free(config->username);

  config->name = NULL;
  config->email = NULL;
  config->username = NULL;
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

  else
  {
    // Unknown section / name, error.
    return 0;
  }

  return 1;
}
