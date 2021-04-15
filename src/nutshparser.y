%{
#pragma GCC diagnostic ignored "-Wwritable-strings"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "src/command_handling.h"
#include "src/global.h"
#include <string.h>

int yylex(void);
int yyerror(char *s);
int yylex_destroy(void);
void unput(char);

int runCommandTable();

%}
%union {char *string;}
%union {char *character;}
%union {char *command;}

%start cmd_line
%token <string> BYE END WORD ERR_GREATERTHAN AMP1
%token <command> CD SETENV PRINTENV UNSETENV ALIAS UNALIAS
%token <character> PIPE LESSTHAN GREATERTHAN AMP
%error-verbose

%%
cmd_line    :
	BYE END 		                        {return handleBYE(); }
  | CD END                            {return handleCDHome();}
  | CD WORD END        			          {return handleCD($2);}
  | SETENV WORD WORD END              {return handleSETENV($2, $3);}
  | PRINTENV oRedB END        	      {return handlePRINTENV();}
  | UNSETENV WORD END       		      {return handleUNSETENV($2);}
  | ALIAS WORD WORD END		            {return handleSetAlias($2, $3);}
  | ALIAS oRedB END     		          {return handleShowAlias();}
  | UNALIAS WORD END		              {return handleUnsetAlias($3);}
  | cmd pipCmds iRed oRed eRed amp END{return runCommandTable();}

amp:
  | AMP                               {enableAmpersand();}

eRed:
  | ERR_GREATERTHAN WORD              {handleErrRed($2, false);}
  | ERR_GREATERTHAN AMP1              {handleErrRed($2, true);}
iRed:
  | LESSTHAN WORD                     {handleInRed($2);}
oRed:
  | GREATERTHAN WORD                  {handleOutRed($2, false);}
  | GREATERTHAN GREATERTHAN WORD      {handleOutRed($3, true);}
oRedB:
  | GREATERTHAN WORD                  {handleOutRedB($2, false);}
  | GREATERTHAN GREATERTHAN WORD      {handleOutRedB($3, true);}

pipCmds:
  | pipCmds pipCmd
pipCmd:
  PIPE cmd

cmd:
  | WORD args                         {handleCommands($1);}
args:
  | args arg
arg:
  WORD                                {handleArgs($1)}

%%

int runCommandTable(){
  if(backgroundEnable){
    return handleCommandTableBG();
  }
  return handleCommandTable();
}

int yyerror(char *s) {
  fprintf(stderr, "%sError:%s %s.\n", RED, RESET, s);
  yylex_destroy();
  return 1;
}