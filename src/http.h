#pragma once

#include <stdlib.h>

// Utility string for cURL
struct cURL_str
{
  char* data;
  size_t length;
};

int cURL_str_init(struct cURL_str* str);
void cURL_str_free(struct cURL_str* str);
void cURL_str_print(struct cURL_str* str);

// Perform an HTTP GET and grab the result to str
// int http_get(char const* url, struct cURL_str* str);
int http_get(char const* url, char** result);
