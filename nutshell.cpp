

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "global.h"
#include "commands.h"
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using namespace std;

char *getcwd(char *buf, size_t size);
int yyparse();

map<string, string> environment;
map<string, string> aliases;

vector<string> curArgs;

void handleBYE(){
    return(exit(1));
}

int handleCD(string newDir){
    char arr[FILENAME_MAX];
    string curDir = getcwd(arr, sizeof(arr));
    string targetDir = curDir + "/" + newDir;

    if (newDir[0] == '/') { // arg is relative path
		targetDir = newDir;
	}

    if(chdir(targetDir.c_str()) == 0) {
        environment["PROMPT"] = getcwd(arr, sizeof(arr));
        return 1;
		}
	else {
        printf("Directory not found\n");
        return 1;
	}
}

int handleCDHome() {
    char arr[FILENAME_MAX];
    string curDir = getcwd(arr, sizeof(arr));
    environment["PROMPT"] = getenv("HOME");
    chdir(getenv("HOME"));
    return 1;
}

int handleCDTilde(string user) {
    struct passwd* pwd;
    pwd = getpwnam(user.c_str());
    if (pwd) {
        environment["PROMPT"] = pwd->pw_dir;
        chdir(getenv("HOME"));
    } else {
        printf("User not found\n");
    }

    return 1;
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

void tokenize(string const &str, const char delim, vector<string> &out)
{
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

int handleCommand(string name){
    pid_t p1 = fork(), parentpid;
    int status = 0;
    int execRes = 0;

    char **argArray;
    int argArrayLength = 0;

    vector<string> paths;

    argArrayLength = 0;
    argArray = (char**) malloc((curArgs.size() + 2) * sizeof(char*));
    argArray[0] = (char *) malloc((name.length()) * sizeof(char));
    strcpy(argArray[0], name.c_str());
    argArrayLength++;
    for (int i = 1; i < (curArgs.size() + 1); i++){
        argArray[i] = (char *) malloc((curArgs[i - 1].length()) * sizeof(char));
        strcpy(argArray[i], curArgs[i - 1].c_str());
        argArrayLength++;
    }
    argArray[curArgs.size() + 1] = NULL;
    tokenize(environment["PATH"], ':', paths);

    if(p1 < 0)
    {
        printf("error...");
    }
    else if(p1 > 0)
    {
        while ((parentpid = wait(&status)) > 0);
        for (int i = 0; i < argArrayLength; i++){
            free(argArray[i]);
            argArray[i] = NULL;
        }
        free(argArray);
        argArray = NULL;
        curArgs.clear();         
    }
    else
    {
        for (int i = 0; i < paths.size(); i++){
            execRes = execv((paths[i] + "/" + name).c_str(), argArray);
        }
        printf("Command not found\n");
        exit(0);
    }   
    return 1;
}

int handleArgs(string word){
    curArgs.push_back(word);
    return 1;
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