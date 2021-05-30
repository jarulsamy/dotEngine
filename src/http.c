#include "http.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Callback for curl fetch
static size_t curl_callback(void *contents, size_t size, size_t nmemb,
                            void *userp)
{
  size_t real_size = size * nmemb;            // calculate buffer size
  struct string *p = (struct string *)userp;  // cast pointer to fetch struct

  // Expand buffer using a temporary pointer to avoid memory leaks
  char *temp = realloc(p->data, p->length + real_size + 1);
  if (temp == NULL)
  {
    fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
    free_string(p);
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

int http_get(char const *url, struct string *str)
{
  CURL *curl;
  CURLcode res;

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
    curl_easy_cleanup(curl);
    if (res == 0)
    {
      return 1;
    }
  }

  return 0;
}
