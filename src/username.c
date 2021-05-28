
#include "username.h"

#include <stdlib.h>
#include <string.h>

#include "parser.h"

void free_configuration(struct configuration* config)
{
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
