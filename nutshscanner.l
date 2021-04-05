%{
#include "nutshparser.tab.h"
#include "commands.h"
#include <string.h>

%}
%array
%option noyywrap
%x string_condition

CHAR                [.A-Za-z0-9!\/_-][.A-Za-z0-9!\/_-]*
ANYCHAR             [)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -][)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -]*

%%

<string_condition>{ANYCHAR}+     { yylval.string = strdup(yytext); return WORD;}
<string_condition>[\"]          {BEGIN(INITIAL);}


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