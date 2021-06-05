#include "http.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int http_str_init(struct http_str *str)
{
  str->length = 0;
  str->data = malloc(str->length + 1);
  if (!str->data)
  {
    return 0;
  }
  str->data[0] = '\0';

  return 1;
}

void http_str_free(struct http_str *str)
{
  str->length = -1;
  free(str->data);
  str->data = NULL;
}

void http_str_print(struct http_str *str)
{
  for (int i = 0; i < str->length; i++)
  {
    putc(str->data[i], stdout);
  }
  putc('\n', stdout);
}

// Callback for curl fetch
static size_t curl_callback(void *contents, size_t size, size_t nmemb,
                            struct http_str *p)
{
  size_t real_size = size * nmemb;  // calculate buffer size

  // Expand buffer using a temporary pointer to avoid memory leaks
  char *temp = realloc(p->data, p->length + real_size + 1);
  if (temp == NULL)
  {
    fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback\n");
    return 0;
  }
  // Assign payload
  p->data = temp;

  // Copy contents to buffer
  memcpy(&(p->data[p->length]), contents, real_size);

  // Set new buffer size
  p->length += real_size;

  // Ensure null termination
  p->data[p->length] = '\0';

  // Return size
  return real_size;
}

// int http_get(char const *url, struct cURL_str *str)
int http_get(char const *url, char **result)
{
  CURL *curl;
  CURLcode res;

  // Temporary string to dynamically fill with HTTP contents
  struct http_str *str = malloc(sizeof(*str));
  if (str == NULL || !http_str_init(str))
  {
    return 0;
  }

  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 1);

    // Fetch the URL
    res = curl_easy_perform(curl);

    // Move from tmp to actual result.
    *result = str->data;

    curl_easy_cleanup(curl);
  }

  free(str);
  if (res == 0)
  {
    return 1;
  }
  return 0;
}
