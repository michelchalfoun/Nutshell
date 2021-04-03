// This is ONLY a demo micro-shell whose purpose is to illustrate the need for and how to handle nested alias substitutions and how to use Flex start conditions.
// This is to help students learn these specific capabilities, the code is by far not a complete nutshell by any means.
// Only "alias name word", "cd word", and "bye" run.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "global.h"
#include <unistd.h>

int yyparse();

int main()
{
    while(1)
    {
        // printf("[%s]>> ", varTable.word[2]);
        printf("hello: ");
        yyparse();
    }

   return 0;
}