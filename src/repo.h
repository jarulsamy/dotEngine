#pragma once

#include <stddef.h>
#include <stdio.h>

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
void free_repo(const struct repo* r);
void repo_print(const struct repo* r);
void repo_pretty_print(const struct repo* r);

#ifndef strdup
char* strdup(const char* c_str);
#endif

// Given a string of JSON from the GitHub API, parse it into repos.
int parse_repos(const char* str, struct repo** repos, size_t* count);

// To and from disk ops
int repo_to_file(FILE* fp, const struct repo* repos, const size_t num);
int repo_from_file(FILE* fp, struct repo** repos, size_t* num);
