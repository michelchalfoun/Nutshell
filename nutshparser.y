%{
// This is ONLY a demo micro-shell whose purpose is to illustrate the need for and how to handle nested alias substitutions and how to use Flex start conditions.
// This is to help students learn these specific capabilities, the code is by far not a complete nutshell by any means.
// Only "alias name word", "cd word", and "bye" run.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// #include "global.h"

int yylex(void);
int yyerror(char *s);
%}

%union {char *string;}

%start cmd_line
%token <string> BYE END STRING

%%
cmd_line    :
	BYE END 		                {exit(1); return 1; }

%%

int yyerror(char *s) {
  printf("%s\n",s);
  return 0;
  }
