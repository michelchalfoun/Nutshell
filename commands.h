#pragma once
#include <string>
using namespace std;

void handleBYE();
int handleCD(string newDir);
int handleCDHome();
int handleCDTilde(string user);
int handleSETENV(string name, string value);
int handlePRINTENV();
int handleUNSETENV(string name);
int handleSetAlias(string name, string value);
int handleShowAlias();
int handleUnsetAlias(string name);

int handleCommand(string name);
int handleArgs(string word);

bool ifAlias(char* name);
const char* subAlias(char* name);

bool ifEnv(char* name);
const char* subEnv(char* name);
