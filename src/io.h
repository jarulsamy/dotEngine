#pragma once

struct config
{
  char* name;
  char* email;
  char* username;
};

void config_init(struct config* c);
void config_free(struct config* c);
int config_ini_handler(void* user, const char* section, const char* name,
                       const char* value);
