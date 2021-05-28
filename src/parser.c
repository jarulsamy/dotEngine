#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

int init_string_base(struct string* str)
{
  str->length = 0;
  str->data = malloc((unsigned)(str->length + 1));
  if (!str->data)
  {
    return 0;
  }
  str->data[0] = '\0';

  return 0;
}

void free_string(struct string* str)
{
  str->length = -1;
  free(str->data);
  str->data = NULL;
}

void print_string(struct string* str)
{
  for (int i = 0; i < str->length; i++)
  {
    putc(str->data[i], stdout);
  }
  putc('\n', stdout);
}

int get_repos(struct string* str, struct repo** repos, size_t* count)
{
  const size_t MAX_REPOS = 250;

  // Track errors as a bool, 0 - error occured, 1 - success.
  int status = 1;

  cJSON* json = cJSON_Parse(str->data);
  cJSON* repo_json = NULL;

  // Default to 250 possible repos, in theory this is the maximum ever possible
  // to be received from the github api.
  // TODO: Grow this array when necessary.
  *repos = malloc(sizeof(struct repo) * MAX_REPOS);
  if (*repos == NULL || json == NULL)
  {
    status = 0;
    goto cleanup;
  }

  *count = 0;
  cJSON_ArrayForEach(repo_json, json)
  {
    if (*count >= MAX_REPOS)
    {
      break;
    }
    struct repo* current = &(*repos)[*count];
    init_repo(current);

    // Name
    cJSON* name = cJSON_GetObjectItemCaseSensitive(repo_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
      current->name = strdup(name->valuestring);
    }

    // Owner
    cJSON* owner = cJSON_GetObjectItemCaseSensitive(repo_json, "owner");
    cJSON* login = cJSON_GetObjectItemCaseSensitive(owner, "login");
    if (cJSON_IsString(login) && (login->valuestring != NULL))
    {
      current->owner = strdup(login->valuestring);
    }

    // Description
    cJSON* description =
        cJSON_GetObjectItemCaseSensitive(repo_json, "description");
    if (cJSON_IsString(description) && (description->valuestring != NULL))
    {
      current->description = strdup(description->valuestring);
    }

    // git_url
    cJSON* git_url = cJSON_GetObjectItemCaseSensitive(repo_json, "git_url");
    if (cJSON_IsString(git_url) && (git_url->valuestring != NULL))
    {
      current->git_url = strdup(git_url->valuestring);
    }

    // ssh_url
    cJSON* ssh_url = cJSON_GetObjectItemCaseSensitive(repo_json, "ssh_url");
    if (cJSON_IsString(name) && (ssh_url->valuestring != NULL))
    {
      current->ssh_url = strdup(ssh_url->valuestring);
    }

    // clone_url
    cJSON* clone_url = cJSON_GetObjectItemCaseSensitive(repo_json, "clone_url");
    if (cJSON_IsString(clone_url) && (clone_url->valuestring != NULL))
    {
      current->clone_url = strdup(clone_url->valuestring);
    }

    // html_url
    cJSON* html_url = cJSON_GetObjectItemCaseSensitive(repo_json, "html_url");
    if (cJSON_IsString(html_url) && (html_url->valuestring != NULL))
    {
      current->html_url = strdup(html_url->valuestring);
    }

    (*count)++;
  }

cleanup:
  cJSON_Delete(json);
  return status;
}

// Shamelessly stolen from glibc, since strdup is not available on all
// compilers.
#ifndef strdup
// Duplicate S, returning an identical malloc'd string.
char* strdup(const char* s)
{
  size_t len = strlen(s) + 1;
  void* new = malloc(len);
  if (new == NULL) return NULL;
  return (char*)memcpy(new, s, len);
}
#endif /* strndup */
