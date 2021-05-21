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

int init_string_cstr(char const* c_str, struct string* str)
{
  size_t len = 0;
  // Get the length of the raw string.
  while (c_str[len] != '\0')
  {
    len++;
  }
  str->length = len;

  // Allocate enough space for the new string.
  str->data = malloc(sizeof(char) * (unsigned)str->length);
  if (!str->data)
  {
    return 0;
  }
  strcpy(str->data, c_str);

  return 1;
}

void free_string(struct string* str)
{
  // Already freed, just return
  if (str->length == 1)
  {
    return;
  }

  str->length = -1;
  free(str->data);
}

void print_string(struct string* str)
{
  for (int i = 0; i < str->length; i++)
  {
    putc(str->data[i], stdout);
  }
  putc('\n', stdout);
}

int get_repos(struct string* str)
{
  //
  cJSON* json = cJSON_Parse(str->data);
  cJSON* repo = NULL;

  int status = 1;
  if (json == NULL)
  {
    const char* error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
      fprintf(stderr, "Error before: %s\n", error_ptr);
      status = 0;
      goto cleanup;
    }
  }

  cJSON_ArrayForEach(repo, json)
  {
    cJSON* full_name = cJSON_GetObjectItemCaseSensitive(repo, "full_name");
    if (cJSON_IsString(full_name) && (full_name->valuestring != NULL))
    {
      printf("%s\n", full_name->valuestring);
    }
  }

cleanup:
  cJSON_Delete(json);
  return status;
}

// Shamelessly stolen from glibc, since strdup is not POSIX compliant.
#ifndef strdup

#ifndef weak_alias
#define __strdup strdup
#endif /* weak alias */

// Duplicate S, returning an identical malloc'd string.
char* __strdup(const char* s)
{
  size_t len = strlen(s) + 1;
  void* new = malloc(len);
  if (new == NULL) return NULL;
  return (char*)memcpy(new, s, len);
}
#ifdef libc_hidden_def
libc_hidden_def(__strdup)
#endif /* libc_hidden_def */

#ifdef weak_alias
    weak_alias(__strdup, strdup)
#endif /* weak alias */

#endif /* strndup */
