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
map<string, string> aliases;

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

int handleSetAlias(string name, string value){
    if (name == value){
        printf("Error, expansion of \"%s\" would create a loop.\n", name.c_str());
        return 1;
    }
    for (auto i = aliases.begin(); i != aliases.end(); i++){
        if (((i->first).c_str() == name) && ((i->first).c_str() == name)){
            printf("Error, expansion of \"%s\" would create a loop.\n", name.c_str());
            return 1;
        }else if ((i->first).c_str() == name){
            i->second = value;
            return 1;
        }
    }
    aliases[name] = value;
    return 1;
}

int handleShowAlias(){
    for (auto i = aliases.begin(); i != aliases.end(); i++){
        printf("%s=%s\n", (i->first).c_str(),(i->second).c_str());
    }
    return 1;
}

int handleUnsetAlias(string name){
    for (auto i = aliases.begin(); i != aliases.end(); i++){
        if (i->first == name){
            aliases.erase(name);
            return 1;
        }
    }
    printf("%s alias not found.", name.c_str());
    return 1;
}

bool ifAlias(char* name){
    if (aliases.find(name) != aliases.end()){
        return 1;
    }
    return 0;
}

const char* subAlias(char* name){
    if (aliases.find(name) != aliases.end()){
        return aliases[name].c_str();
    }
    return name;
}

bool ifEnv(char* name){
    if (environment.find(name) != environment.end()){
        return 1;
    }
    return 0;
}

const char* subEnv(char* name){
    if (environment.find(name) != environment.end()){
        return environment[name].c_str();
    }
    return name;
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
        printf("%s [%s] >> %s", BOLDCYAN, environment["PROMPT"].c_str(), RESET);
        yyparse();
    }

   return 0;
}