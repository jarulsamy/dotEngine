#pragma once

struct configuration
{
  char* name;
  char* email;
  char* username;
};

void init_configuration(struct configuration* config);
void free_configuration(struct configuration* config);

int handler(void* user, const char* section, const char* name,
            const char* value);
