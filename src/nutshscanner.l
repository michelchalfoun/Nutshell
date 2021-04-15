%{
#pragma GCC diagnostic ignored "-Wdeprecated-register" 

#include "nutshparser.tab.h"
#include "src/command_handling.h"
#include <string.h>
#include <string>

bool first = false;
char envString[100];

void falsifyFirst(){
    first = false;
}

%}
%array
%option noyywrap
%x string_condition
%x envVar_condition
%x envVarQuotes_condition

CHAR                [.A-Za-z0-9!\?:\*\/_-]*
ANYCHAR             [a-zA-Z\/.0-9\-&*.?:\;\'\<\>\, ~`_=\+\[\]@#%()\^]

%%

<string_condition>"${"              {BEGIN(envVarQuotes_condition);}
<string_condition>[\"]              {BEGIN(INITIAL);yylval.string = strdup(envString);return WORD;}
<string_condition>{ANYCHAR}+        { 
                                        first = false;
                                        strcat(envString, yytext);
                                    }
<string_condition>.                 { }

<envVarQuotes_condition>[}]         {BEGIN(string_condition);}
<envVarQuotes_condition>{ANYCHAR}+  {
                                        if(ifEnv(yytext)){
                                            strcat(envString, strdup(subEnv(yytext)));
                                        };
                                    }
<envVarQuotes_condition>.           { }

<envVar_condition>{CHAR}+           { 
                                        if(ifEnv(yytext)){
                                            char *yycopy = strdup(subEnv(yytext));
                                                for ( int i = strlen(subEnv(yytext)) - 1; i >= 0; --i )
                                                    unput( yycopy[i] );
                                                free(yycopy);
                                        }else{
                                            yylval.string = strdup(yytext); 
                                            return WORD; 
                                        };
                                    }
<envVar_condition>[}]               {BEGIN(INITIAL);}

[ \t\r]		        { }
bye                 { first = false; return BYE; }
cd		            { first = false; return CD;}
setenv		        { first = false; return SETENV;}
printenv		    { first = false; return PRINTENV;}
unsetenv		    { first = false; return UNSETENV;}
alias               { first = false; return ALIAS; }
unalias             { first = false; return UNALIAS; }
\n		            { first = true; return END; }
[\"]                { strcpy(envString, ""); BEGIN(string_condition); }
"${"                { first = false; BEGIN(envVar_condition); }
"|"                 { first = false; return PIPE; }
"2>"                { first = false; return ERR_GREATERTHAN; }
"&1"                { first = false; return AMP1; }
"&"                 { first = false; return AMP; }
"<"                 { first = false; return LESSTHAN; }
">"                 { first = false; return GREATERTHAN; }
{CHAR}+             { 
                        if(ifAlias(yytext) && first){
                            first = false;
                            char *yycopy = strdup( subAlias(yytext) );
                                for ( int i = strlen(subAlias(yytext)) - 1; i >= 0; --i )
                                    unput( yycopy[i] );
                                free( yycopy );
                        }else{
                            first = false;
                            yylval.string = strdup(yytext); 
                            return WORD; 
                        };
                    }
.                   { }
%%