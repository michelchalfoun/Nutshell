#pragma once

#include "stdbool.h"
#include <limits.h>

struct evTable {
   char var[128][100];
   char word[128][100];
};
inline struct evTable varTable;
inline char cwd[PATH_MAX];
inline int varIndex;