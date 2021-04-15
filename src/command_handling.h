#pragma once

#include "global.h"
#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <pwd.h>
#include <fcntl.h>
#include "../nutshparser.tab.h"

using namespace std;

int yyerror(char *s);

int handleBYE();
int enableAmpersand();

int handleCD(string newDir);
int handleCDHome();
int handleCDTilde(string user);

int handleSETENV(string name, string value);
int handlePRINTENV();
int handleUNSETENV(string name);
bool ifEnv(char* name);
const char* subEnv(char* name);

int handleSetAlias(string name, string value);
int handleShowAlias();
int handleUnsetAlias(string name);
bool ifAlias(char* name);
const char* subAlias(char* name);

int handleCommand(string name, vector<string> args, bool pipeIn, bool pipeOut, string inputFile, string outputFile, bool append, string errOutput);
int handleCommandTable();
int handleCommandTableBG();

int handleArgs(string word);
int handleCommands(string word);

int handleInRed(string name);
int handleOutRed(string name, bool append);
int handleOutRedB(string name, bool append);
int handleErrRed(string name, bool stdOutput);
int handleErrRedB(string name, bool stdOutput);

char* autofill(char* partialDir);
char* autofillTilde(char* partialDir);