#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "global.h"
#include "commands.h"
#include <unistd.h>
#include <iostream>
#include <map>

using namespace std;

char *getcwd(char *buf, size_t size);
int yyparse();

map<string, string> environment;

int handleCD(string newDir){
    char arr[FILENAME_MAX];
    string curDir = getcwd(arr, sizeof(arr));
    
    string targetDir = curDir + "/" + newDir;
    if (newDir[0] == '/') { // arg is relative path
		targetDir = newDir;
	}

    if(chdir(targetDir.c_str()) == 0) {
        environment["PROMPT"] = targetDir;
        return 1;
		}
	else {
        printf("Directory not found\n");
        return 1;
	}
}

int handleSETENV(string name, string value){
    if ((name.length() > 0) && (value.length() > 0)){
        environment[name] = value;
        return 1;
    }else{
        printf("Invalid input.");
        return 1;
    }
}

int handlePRINTENV(){
    for (auto i = environment.begin(); i != environment.end(); i++){
        printf("%s:%s\n", (i->first).c_str(), (i->second).c_str());
    }
    return 1;
}

int handleUNSETENV(string name){
    if (name.length() > 0){
        environment.erase(name);
        return 1;
    }else{
        printf("Invalid input.");
        return 1;
    }
}

int main()
{
    getcwd(cwd, sizeof(cwd));

    environment["PWD"] = cwd;
    environment["HOME"] = cwd;
    environment["PROMPT"] = cwd;
    environment["PATH"] = ".:/bin";

    system("clear");
    while(1)
    {
        printf("%s [%s] >> %s", GREEN, environment["PROMPT"].c_str(), RESET);
        yyparse();
    }

   return 0;
}