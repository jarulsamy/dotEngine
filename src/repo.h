#pragma once

#include <stddef.h>
#include <stdio.h>

int valid_cache(const char* filename);

#ifndef strdup
char* strdup(const char* c_str);
#endif

struct repo
{
  // Basic information
  char* name;
  char* owner;
  char* description;

  // Various clone urls
  char* git_url;
  char* ssh_url;
  char* clone_url;

  // https://github.com/USERNAME/REPO
  char* html_url;
};

void repo_init(struct repo* r);
void repo_free(const struct repo* r);

void repo_print(const struct repo* r);
void repo_print_pretty(const struct repo* r);
void repo_print_zshsuggestion(const struct repo* r, const size_t num);

// Given a string of JSON from the GitHub API, parse it into repos.
int repo_parse(const char* str, struct repo** repos, size_t* count);

// To and from disk ops
int repo_to_file(FILE* fp, const struct repo* repos, const size_t num);
int repo_from_file(FILE* fp, struct repo** repos, size_t* num);
