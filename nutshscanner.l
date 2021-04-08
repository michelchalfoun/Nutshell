%{
#include "nutshparser.tab.h"
#include "commands.h"
#include <string.h>

%}
%array
%option noyywrap
%x string_condition
%x envVar_condition

CHAR                [.A-Za-z0-9!\/_-][.A-Za-z0-9!\/_-]*
ANYCHAR             [^"]*

%%

<string_condition>{ANYCHAR}+    { yylval.string = strdup(yytext); return WORD;}
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
bye                 { return BYE; }
cd		            { return CD;}
setenv		        { return SETENV;}
printenv		    { return PRINTENV;}
unsetenv		    { return UNSETENV;}
alias               { return ALIAS; }
unalias             { return UNALIAS; }
"\n"		        { return END; }
[\"]                { BEGIN(string_condition); }
"${"                { BEGIN(envVar_condition); }
"~"                 { return TILDE; }
{CHAR}+             { 
                        if(ifAlias(yytext)){
                            char *yycopy = strdup( subAlias(yytext) );
                                for ( int i = strlen(subAlias(yytext)) - 1; i >= 0; --i )
                                    unput( yycopy[i] );
                                free( yycopy );
                        }else{
                            yylval.string = strdup(yytext); 
                            return WORD; 
                        };
                    }
%%