#pragma once

#include "global.h"
#include <vector>
#include <map>
#include <glob.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void expandPathEnv();
void splitString(string const &str, const char delim, vector<string> &out);
vector<string> getWildcardArgs(string arg);