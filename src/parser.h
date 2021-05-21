#pragma once

struct string
{
  char* data;
  int length;
};

int init_string_base(struct string* str);
int init_string_cstr(char const* c_str, struct string* str);
void free_string(struct string* str);
void print_string(struct string* str);

// Given a string of JSON from the GitHub API, parse it into just the repo
// names.
int get_repos(struct string* str);

char* strdup(const char* c_str);
