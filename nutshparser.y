%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commands.h"
#include <string.h>

int yylex(void);
int yyerror(char *s);
int runCD(char* arg);
int runCDHome();
int runCDTilde(char* user);

int runSETENV(char* name, char* value);
int runPRINTENV();
int runUNSETENV(char* name);

int runSetAlias(char *name, char *value);
int runShowAlias();
int runUnsetAlias(char *name);
%}

%union {char *string;}

%start cmd_line
%token <string> BYE END STRING CD WORD TILDE
%token <command> SETENV PRINTENV UNSETENV ALIAS UNALIAS
%error-verbose

%%
cmd_line    :
	BYE END 		                  {exit(1); return 1; }
  | CD END                      {runCDHome(); return 1;}
  | CD TILDE END                {runCDHome(); return 1;}
  | CD TILDE WORD END           {runCDTilde($3); return 1;}
  | CD WORD END        			    {runCD($2); return 1;}
  | SETENV WORD WORD END        {runSETENV($2, $3); return 1;}
  | PRINTENV END        			  {runPRINTENV(); return 1;}
  | UNSETENV WORD END       		{runUNSETENV($2); return 1;}
  | ALIAS WORD WORD END		      {runSetAlias($2, $3); return 1;}
  | ALIAS END		                {runShowAlias(); return 1;}
  | UNALIAS WORD END		        {runUnsetAlias($2); return 1;}
%%

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

int yyerror(char *s) {
  printf("%s\n",s);
  return 0;
}
