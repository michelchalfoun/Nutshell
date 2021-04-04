%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "global.h"
#include "commands.h"
#include <string.h>

int yylex(void);
int yyerror(char *s);
int runCD(char* arg);
%}

%union {char *string;}

%start cmd_line
%token <string> BYE END STRING CD WORD
%error-verbose

%%
cmd_line    :
	BYE END 		                {exit(1); return 1; }
  | CD WORD END        			{runCD($2); return 1;}
%%

int runCD(char* arg) {
	return handleCD(arg);
}

int yyerror(char *s) {
  printf("%s\n",s);
  return 0;
}
