%{
#pragma GCC diagnostic ignored "-Wdeprecated-register" 

#include "nutshparser.tab.h"
#include "commands.h"
#include <string.h>

bool first = false;

void falsifyFirst(){
    first = false;
}

%}
%array
%option noyywrap
%x string_condition
%x envVar_condition

CHAR                [.A-Za-z0-9!\/_-][.A-Za-z0-9!\/_-]*
ANYCHAR             [^"]*

%%


<string_condition>{ANYCHAR}+       { 
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
<string_condition>[\"]          {BEGIN(INITIAL);}

<envVar_condition>{CHAR}+       { 
                                    if(ifEnv(yytext)){
                                        char *yycopy = strdup( subEnv(yytext) );
                                            for ( int i = strlen(subEnv(yytext)) - 1; i >= 0; --i )
                                                unput( yycopy[i] );
                                            free( yycopy );
                                    }else{
                                        yylval.string = strdup(yytext); 
                                        return WORD; 
                                    };
                                }
<envVar_condition>[}]           {BEGIN(INITIAL);}

[ ]		            { }
bye                 { first = false; return BYE; }
cd		            { first = false; return CD;}
setenv		        { first = false; return SETENV;}
printenv		    { first = false; return PRINTENV;}
unsetenv		    { first = false; return UNSETENV;}
alias               { first = false; return ALIAS; }
unalias             { first = false; return UNALIAS; }
\n		            { first = true; return END; }
[\"]                { BEGIN(string_condition); }
"${"                { first = false; BEGIN(envVar_condition); }
"~"                 { return TILDE; }
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
%%