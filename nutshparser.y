%{
#pragma GCC diagnostic ignored "-Wwritable-strings"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commands.h"
#include "global.h"
#include <string.h>

int yylex(void);

void unput(char);

int yyerror(char *s);
int runBYE();
int runCD(char* arg);
int runCDHome();
int runCDTilde(char* user);

int runSETENV(char* name, char* value);
int runPRINTENV();
int runUNSETENV(char* name);

int runSetAlias(char *name, char *value);
int runShowAlias();
int runUnsetAlias(char *name);

int runCommand(char *name);
int runCommandTable();

int fillArgs(char *name);
int fillCommands(char *name);

int runInRed(char *name);
int runOutRed(char *name, bool append);

int runErrRed(char *name, bool stdOutput);

int runAmpersand();

extern void scan_string(const char* str);
%}

%union {char *string;}
%union {char *command;}

%start cmd_line
%token <string> BYE END STRING CD WORD TILDE
%token <command> SETENV PRINTENV UNSETENV ALIAS UNALIAS PIPE LESSTHAN GREATERTHAN ERR_GREATERTHAN AMP1 AMP
%error-verbose

%%
cmd_line    :
	BYE END 		                        {runBYE(); return 1; }
  | CD END                            {runCDHome(); return 1;}
  /* | CD TILDE END                     {runCDHome(); return 1;}
  | CD TILDE WORD END                 {runCDTilde($3); return 1;} */
  | CD WORD END        			          {runCD($2); return 1;}
  | SETENV WORD WORD END              {runSETENV($2, $3); return 1;}
  | PRINTENV END        			        {runPRINTENV(); return 1;}
  | UNSETENV WORD END       		      {runUNSETENV($2); return 1;}
  | ALIAS WORD WORD END		            {runSetAlias($2, $3); return 1;}
  | ALIAS END		                      {runShowAlias(); return 1;}
  | UNALIAS WORD END		              {runUnsetAlias($2); return 1;}
  | cmd pipCmds iRed oRed eRed amp END{runCommandTable(); return 1;}

amp:
  | AMP                               {runAmpersand();}

eRed:
  | ERR_GREATERTHAN WORD              {runErrRed($2, false);}
  | ERR_GREATERTHAN AMP1              {runErrRed($2, false);}

iRed:
  | LESSTHAN WORD                     {runInRed($2);}

oRed:
  | GREATERTHAN WORD                  {runOutRed($2, false);}
  | GREATERTHAN GREATERTHAN WORD      {runOutRed($3, true);}

pipCmds:
  | pipCmds pipCmd                    {}

pipCmd:
  PIPE cmd                            {}

cmd:
  | WORD args                         {fillCommands($1);}

args:
  | args arg                          {}

arg:
  WORD                                {fillArgs($1)}

%%

int runBYE(){
  handleBYE();
  return 1;
}

int runCD(char* arg) {
	return handleCD(arg);
}

int runCDHome() {
  return handleCDHome();
}

int runCDTilde(char* user) {
  return  handleCDTilde(user);
}

int runSETENV(char* name, char* value) {
	return handleSETENV(name, value);
}

int runPRINTENV() {
	return handlePRINTENV();
}

int runUNSETENV(char* name) {
	return handleUNSETENV(name);
}

int runSetAlias(char *name, char *value) {
  return handleSetAlias(name, value);
}

int runShowAlias() {
  return handleShowAlias();
}

int runUnsetAlias(char *name) {
  return handleUnsetAlias(name);
}

/* int runCommand(char *name) {
  return handleCommand(name);
} */

int runCommandTable(){
  if(backgroundEnable){
    return handleCommandTableBG();
  }
  return handleCommandTable();
}

int fillArgs(char *name){
  return handleArgs(name);
}

int fillCommands(char *name){
  return handleCommands(name);
}

int runInRed(char *name){
  return handleInRed(name);
}

int runOutRed(char *name, bool append){
  return handleOutRed(name, append);
}

int runErrRed(char *name, bool stdOutput){
  return handleErrRed(name, stdOutput);
}

int runAmpersand(){
  return enableAmpersand();
}

int yyerror(char *s) {
  printf("%s\n",s);
  return 0;
}
