#include "repo.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "cJSON.h"

int valid_cache(const char* filename)
{
  struct stat cache;
  struct timespec current;

  // If the cache is not available, or we can't check the current time.
  if (stat(filename, &cache) != 0 ||
      clock_gettime(CLOCK_REALTIME, &current) < 0)
  {
    return 0;
  }

#ifndef NDEBUG
  fprintf(stderr, "The cache is %d seconds old.\n",
          abs(difftime(cache.st_mtime, current.tv_sec)));
#endif

  // If the modification time of the cache file is over 1 day old
  if (abs(difftime(cache.st_mtime, current.tv_sec)) > 60 * 60 * 24)
  {
    return 0;
  }

  return 1;
}

// Shamelessly stolen from glibc.
#ifndef strdup
// Duplicate S, returning an identical malloc'd string.
char* strdup(const char* s)
{
  size_t len = strlen(s) + 1;
  void* new = malloc(len);
  if (new == NULL) return NULL;
  return (char*)memcpy(new, s, len);
}
#endif /* strdup */

static char* trim(char* s)
{
  int l = strlen(s);

  while (isspace(s[l - 1])) --l;
  while (*s && isspace(*s)) ++s, --l;

  return strndup(s, l);
}

static char* clean(char* s)
{
  char* str = trim(s);

  // Remove apostophes, messes with zsh auto completion.
  for (int i = 0, j; str[i] != '\0'; ++i)
  {
    while (str[i] == '"' && !(str[i] == '\0'))
    {
      for (j = i; str[j] != '\0'; ++j)
      {
        str[j] = str[j + 1];
      }
      str[j] = '\0';
    }
  }

  return str;
}

// Initialize with all NULL values
void repo_init(struct repo* r)
{
  r->name = NULL;
  r->owner = NULL;
  r->description = NULL;
  r->git_url = NULL;
  r->ssh_url = NULL;
  r->clone_url = NULL;
  r->html_url = NULL;
}

void repo_free(const struct repo* r)
{
  free(r->name);
  free(r->owner);
  free(r->description);
  free(r->git_url);
  free(r->ssh_url);
  free(r->clone_url);
  free(r->html_url);
}

void repo_print(const struct repo* r)
{
  if (r == NULL)
  {
    return;
  }

  printf("%s\n", r->name);
}

void repo_print_pretty(const struct repo* r)
{
  if (r == NULL)
  {
    return;
  }

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

void repo_print_zshsuggestion(const struct repo* repos, const size_t num)
{
  /* Sample ZSH autcompletion entry

  _dotEngine() {
    _describe 'command' "($(/PATH/TO/dotEngine -z))"
  }
  compdef _dotEngine dotEngine

  Substitute dotEngine with whatever command you may like.
  */

  if (repos == NULL)
  {
    return;
  }

  for (size_t i = 0; i < num; i++)
  {
    char* name = repos[i].name;
    char* descrip = repos[i].description;

    if (name == NULL)
    {
      continue;
    }
    if (descrip == NULL || strcmp(descrip, "(null)") == 0)
    {
      descrip = name;
    }

    // printf("%s\n\t%s\n", name, descrip);
    printf("\"%s:%s\"\n", name, descrip);
  }
  putc('\n', stdout);
}

int repo_parse(const char* str, struct repo** repos, size_t* count)
{
  const size_t MAX_REPOS = 250;

  // Track errors as a bool, 0 - error occured, 1 - success.
  int status = 1;

  cJSON* json = cJSON_Parse(str);
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
    repo_init(current);

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

// Print each member of the struct to a newline
int repo_to_file(FILE* fp, const struct repo* repos, const size_t num)
{
  // Handle invalid inputs
  if (fp == NULL || repos == NULL)
  {
    return 0;
  }

  for (size_t i = 0; i < num; i++)
  {
    fprintf(fp, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n", repos[i].name, repos[i].owner,
            repos[i].description, repos[i].git_url, repos[i].ssh_url,
            repos[i].clone_url, repos[i].html_url);
  }

  return 1;
}

// Load following the write format of repo_to_file
int repo_from_file(FILE* fp, struct repo** repos, size_t* num)
{
  const size_t BUFFER_SIZE = 1024;

  // Start off by allocating enough space for 10 entries
  size_t num_allocated = 10;
  *num = 0;
  *repos = malloc(sizeof(struct repo) * num_allocated);
  if (*repos == NULL)
  {
    return 0;
  }

  char buffer_name[BUFFER_SIZE];
  char buffer_owner[BUFFER_SIZE];
  char buffer_description[BUFFER_SIZE];
  char buffer_git_url[BUFFER_SIZE];
  char buffer_ssh_url[BUFFER_SIZE];
  char buffer_clone_url[BUFFER_SIZE];
  char buffer_html_url[BUFFER_SIZE];

  char* status = NULL;
  do
  {
    // Allocate more memory if need be
    if (*num >= num_allocated)
    {
      num_allocated *= 2;
      *repos = realloc(*repos, sizeof(struct repo) * num_allocated);
      if (*repos == NULL)
      {
        return 0;
      }
    }

    // Load all the fields
    status = fgets(buffer_name, BUFFER_SIZE, fp);
    if (status == NULL)
    {
      break;
    }

    status = fgets(buffer_owner, BUFFER_SIZE, fp);
    status = fgets(buffer_description, BUFFER_SIZE, fp);
    status = fgets(buffer_git_url, BUFFER_SIZE, fp);
    status = fgets(buffer_ssh_url, BUFFER_SIZE, fp);
    status = fgets(buffer_clone_url, BUFFER_SIZE, fp);
    status = fgets(buffer_html_url, BUFFER_SIZE, fp);

    // Strip all the newlines
    buffer_name[strcspn(buffer_name, "\n")] = '\0';
    buffer_owner[strcspn(buffer_owner, "\n")] = '\0';
    buffer_description[strcspn(buffer_description, "\n")] = '\0';
    buffer_git_url[strcspn(buffer_git_url, "\n")] = '\0';
    buffer_ssh_url[strcspn(buffer_ssh_url, "\n")] = '\0';
    buffer_clone_url[strcspn(buffer_clone_url, "\n")] = '\0';
    buffer_html_url[strcspn(buffer_html_url, "\n")] = '\0';

    // Copy them to the structs
    (*repos)[*num].name = clean(buffer_name);
    (*repos)[*num].owner = clean(buffer_owner);
    (*repos)[*num].description = clean(buffer_description);
    (*repos)[*num].git_url = clean(buffer_git_url);
    (*repos)[*num].ssh_url = clean(buffer_ssh_url);
    (*repos)[*num].clone_url = clean(buffer_clone_url);
    (*repos)[*num].html_url = clean(buffer_html_url);

    (*num)++;
  } while (status != NULL);

  return 1;
}
