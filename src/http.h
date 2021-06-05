#pragma once

#include <stdlib.h>

// Utility string for cURL
struct http_str
{
  char* data;
  size_t length;
};

int http_str_init(struct http_str* str);
void http_str_free(struct http_str* str);
void http_str_print(struct http_str* str);

// Perform an HTTP GET and grab the result to str
// int http_get(char const* url, struct cURL_str* str);
int http_get(char const* url, char** result);
