%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commands.h"
#include <string.h>

int yylex(void);
int yylex_destroy(void);

typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char *str);
void yypush_buffer_state(YY_BUFFER_STATE buffer);
void yypop_buffer_state();
void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
void yy_flush_buffer(YY_BUFFER_STATE buffer);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
// int yylex_destroy(YY_BUFFER_STATE buffer);

void unput(char);

int yyerror(char *s);
int runBYE();
int runCD(char* arg);

int runSETENV(char* name, char* value);
int runPRINTENV();
int runUNSETENV(char* name);

int runSetAlias(char *name, char *value);
int runShowAlias();
int runUnsetAlias(char *name);

int runCommand(char *name);

extern void scan_string(const char* str);
%}

%union {char *string;}
%union {char *command;}

%start cmd_line
%token <string> BYE END STRING CD WORD
%token <command> SETENV PRINTENV UNSETENV ALIAS UNALIAS
%error-verbose

%%
cmd_line    :
	BYE END 		                  {runBYE(); return 1; }
  | CD WORD END        			    {runCD($2); return 1;}
  | SETENV WORD WORD END        {runSETENV($2, $3); return 1;}
  | PRINTENV END        			  {runPRINTENV(); return 1;}
  | UNSETENV WORD END       		{runUNSETENV($2); return 1;}
  | ALIAS WORD WORD END		      {runSetAlias($2, $3); return 1;}
  | ALIAS END		                {runShowAlias(); return 1;}
  | UNALIAS WORD END		        {runUnsetAlias($2); return 1;}
  | WORD END                    {runCommand($1); return 1;}
  | WORD args END               {runCommand($1); return 1;}

args:
  | args arg                    {}

arg:
  WORD                          {handleArgs($1)}

%%

int runBYE(){
  handleBYE();
  return 1;
}

int runCD(char* arg) {
	return handleCD(arg);
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

int runCommand(char *name) {
  return handleCommand(name);
}

int yyerror(char *s) {
  printf("%s\n",s);
  return 0;
}
