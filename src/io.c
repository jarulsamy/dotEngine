#include "io.h"

#include <stdlib.h>
#include <string.h>

char* concat(const char* s1, const char* s2)
{
  size_t s1_len = strlen(s1);
  size_t s2_len = strlen(s2);

  char* res = malloc(s1_len + s2_len + 1);
  if (res == NULL)
  {
    return NULL;
  }

  memcpy(res, s1, s1_len);
  memcpy(res + s1_len, s2, s2_len + 1);

  return res;
}

void config_init(struct config* c)
{
  c->name = NULL;
  c->email = NULL;
  c->username = NULL;
  c->password = NULL;
}

void config_free(struct config* c)
{
  free(c->name);
  free(c->email);
  free(c->username);
  free(c->password);

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
  else if (MATCH("dotEngine", "username"))
  {
    pconfig->username = strdup(value);
  }
  else if (MATCH("dotEngine", "password"))
  {
    pconfig->password = strdup(value);
  }
  // Unknown section / name, error.
  else
  {
    return 0;
  }

  return 1;
}
