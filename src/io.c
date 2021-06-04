#include "io.h"

#include <stdlib.h>
#include <string.h>

void config_init(struct config* c)
{
  c->name = NULL;
  c->email = NULL;
  c->username = NULL;
}

void config_free(struct config* c)
{
  free(c->name);
  free(c->email);
  free(c->username);

  // Set all members to NULL
  config_init(c);
}

int config_ini_handler(void* user, const char* section, const char* name,
                       const char* value)
{
  struct config* pconfig = (struct config*)user;

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
