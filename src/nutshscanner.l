%{
#pragma GCC diagnostic ignored "-Wdeprecated-register" 

#include "nutshparser.tab.h"
#include "src/command_handling.h"
#include <string.h>
#include <string>

bool first = true;
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
%x tilde_condition
%x autofill_condition

/* CHAR                [.A-Za-z0-9~!\?:\*\/_-]* */
CHAR                [.A-Za-z0-9!\?:\*\/_-][.A-Za-z0-9!~\?:\*\/_-]*
TILDECHAR           [.A-Za-z0-9!\?:\*_-]*
ANYCHAR             [a-zA-Z\/.0-9\-&*.?:\;\'\<\>\, ~`_=\+\[\]@#%()\^]

%%

<string_condition>"${"              {BEGIN(envVarQuotes_condition);}
<string_condition>[\"]              {BEGIN(INITIAL);
                                        yylval.string = strdup(envString);
                                        return WORD;
                                    }
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

<envVar_condition>[ \t\r]               {}
<envVar_condition>{ANYCHAR}+           { 
                                        printf("in expansion text: %s\n", yytext);
                                        if(ifEnv(yytext)){
                                            char *yycopy = strdup(subEnv(yytext));
                                            for ( int i = strlen(subEnv(yytext)) - 1; i >= 0; --i )
                                                unput( yycopy[i] );
                                            free(yycopy);
                                            BEGIN(INITIAL);
                                        }else{
                                            yylval.string = strdup(""); 
                                            return WORD;
                                        };
                                    }
<envVar_condition>[}]               { BEGIN(INITIAL);}

<tilde_condition>[ \t\r]            {
                                        char* userDir = getenv("HOME");
                                        char *yycopy = strdup(userDir);
                                        for ( int i = strlen(yycopy) - 1; i >= 0; --i )
                                            unput( yycopy[i] );
                                        free(yycopy);
                                        BEGIN(INITIAL);
                                    }

<tilde_condition>\n                 {
                                        char* userDir = getenv("HOME");
                                        char *yycopy = strdup(userDir);
                                        strcat(yycopy, "\n");
                                        for ( int i = strlen(yycopy) - 1; i >= 0; --i )
                                            unput( yycopy[i] );
                                        free(yycopy);
                                        BEGIN(INITIAL);
                                    }
<tilde_condition>{TILDECHAR}+       {
                                        printf("in here\n");
                                        struct passwd* userName;
                                        userName = getpwnam(strdup(yytext));
                                        if (userName) {
                                            char *yycopy = strdup(userName->pw_dir);
                                            for ( int i = strlen(yycopy) - 1; i >= 0; --i )
                                                unput( yycopy[i] );
                                            free(yycopy);
                                            BEGIN(INITIAL);
                                        } else {
                                            yylval.string = strdup("NONEXISTING");
                                            BEGIN(INITIAL); return WORD;
                                        }
                                    }
<tilde_condition>{TILDECHAR}+\033   {
                                        struct passwd* userName;
                                        yytext[strlen(yytext) - 1] = '\0';
                                        char* foundDir = autofillTilde(yytext);
                                        userName = getpwnam(strdup(foundDir));
                                        if (userName) {
                                            char *yycopy = strdup(userName->pw_dir);
                                            for ( int i = strlen(yycopy) - 1; i >= 0; --i )
                                                unput( yycopy[i] );
                                            free(yycopy);
                                            BEGIN(INITIAL);
                                        } else {
                                            yylval.string = strdup("NONEXISTING");
                                            BEGIN(INITIAL); return WORD;
                                        }
                                    }
<tilde_condition>\/                 {BEGIN(INITIAL);}

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
"${"                { printf("bool val: %d\n", first);BEGIN(envVar_condition); }
"|"                 { first = false; return PIPE; }
"2>"                { first = false; return ERR_GREATERTHAN; }
"&1"                { first = false; return AMP1; }
"&"                 { first = false; return AMP; }
"<"                 { first = false; return LESSTHAN; }
">"                 { first = false; return GREATERTHAN; }
"~"                 { BEGIN(tilde_condition); }
{CHAR}+\033$         { 
                        yytext[strlen(yytext) - 1] = '\0';
                        char* foundDir = autofill(yytext);
                        char *yycopy = strdup(foundDir);
                        for ( int i = strlen(foundDir) - 1; i >= 0; --i )
                            unput( yycopy[i] );
                        free( yycopy );
                    }
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