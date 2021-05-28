#pragma once

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

void init_repo(struct repo* r);
void free_repo(const struct repo* r);
void print_repo(const struct repo* r);
