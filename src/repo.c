#include "repo.h"

#include <stdio.h>
#include <stdlib.h>

// Initialize with all NULL values
void init_repo(struct repo* r)
{
  r->name = NULL;
  r->owner = NULL;
  r->description = NULL;
  r->git_url = NULL;
  r->ssh_url = NULL;
  r->clone_url = NULL;
  r->html_url = NULL;
}

void free_repo(const struct repo* r)
{
  free(r->name);
  free(r->owner);
  free(r->description);
  free(r->git_url);
  free(r->ssh_url);
  free(r->clone_url);
  free(r->html_url);
}

void print_repo(const struct repo* r)
{
  printf("====================\n");
  printf("Name: %s\n", r->name);
  printf("Owner: %s\n", r->owner);
  printf("Description: %s\n", r->description);
  printf("git_url: %s\n", r->git_url);
  printf("ssh_url: %s\n", r->ssh_url);
  printf("clone_url: %s\n", r->clone_url);
  printf("html_url: %s\n", r->html_url);
  printf("====================\n");
}
