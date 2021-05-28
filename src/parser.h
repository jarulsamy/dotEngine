#pragma once

#include <stddef.h>

#include "repo.h"

// Utility string for cURL
struct string
{
  char* data;
  int length;
};
int init_string_base(struct string* str);
void free_string(struct string* str);
void print_string(struct string* str);

// Given a string of JSON from the GitHub API, parse it into repos.
int get_repos(struct string* str, struct repo** repos, size_t* count);

#ifndef strdup
char* strdup(const char* c_str);
#endif
