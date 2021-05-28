#pragma once

#include <stdlib.h>

#include "parser.h"

// Perform an HTTP GET and grab the result to str
int http_get(char const *url, struct string *str);
