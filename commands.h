#pragma once
#include <string>
using namespace std;

int handleCD(string newDir);
int handleSETENV(string name, string value);
int handlePRINTENV();
int handleUNSETENV(string name);
int handleSetAlias(string name, string value);
int handleShowAlias();
int handleUnsetAlias(string name);

bool ifAlias(char* name);
const char* subAlias(char* name);
