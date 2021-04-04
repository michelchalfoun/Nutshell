#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "global.h"
#include "commands.h"
#include <unistd.h>
#include <iostream>

using namespace std;

char *getcwd(char *buf, size_t size);
int yyparse();

int handleCD(string newDir){
    char arr[FILENAME_MAX];
    string curDir = getcwd(arr, sizeof(arr));
    
    string targetDir = curDir + "/" + newDir;
    if (newDir[0] == '/') { // arg is relative path
		targetDir = newDir;
	}
    
    if(chdir(targetDir.c_str()) == 0) {
        printf("Directory found\n");
        return 1;
		}
	else {
        printf("Directory not found\n");
        return 1;
	}
    return 0;
}

int main()
{
    varIndex = 0;
    getcwd(cwd, sizeof(cwd));

    strcpy(varTable.var[varIndex], "PWD");
    strcpy(varTable.word[varIndex], cwd);
    varIndex++;
    strcpy(varTable.var[varIndex], "HOME");
    strcpy(varTable.word[varIndex], cwd);
    varIndex++;
    strcpy(varTable.var[varIndex], "PROMPT");
    strcpy(varTable.word[varIndex], cwd);
    varIndex++;
    strcpy(varTable.var[varIndex], "PATH");
    strcpy(varTable.word[varIndex], ".:/bin");
    varIndex++;

    system("clear");
    while(1)
    {
        printf("[%s]>> ", varTable.word[2]);
        yyparse();
    }

   return 0;
}