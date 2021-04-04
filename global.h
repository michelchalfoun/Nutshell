#pragma once

#include "stdbool.h"
#include <limits.h>

struct evTable {
   char var[128][100];
   char word[128][100];
};
struct evTable varTable;
char cwd[PATH_MAX];
int varIndex;