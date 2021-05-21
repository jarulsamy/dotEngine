#pragma once

struct configuration
{
  char* name;
  char* email;
  char* username;
};

void free_configuration(struct configuration* config);

int handler(void* user, const char* section, const char* name,
            const char* value);
