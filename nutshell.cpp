

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
#include <fcntl.h>
#include <fstream>

using namespace std;

char *getcwd(char *buf, size_t size);
int yyparse();

map<string, string> environment;
map<string, string> aliases;

vector<string> curArgs;
vector<CommandInfo> cmdTable;

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

void expandPathEnv(){
    vector<string> paths;
    tokenize(environment["PATH"], ':', paths);
    string pathString = "";
    for (int i = 0; i < paths.size(); i++){
        if (paths[i][0] == '~'){
            paths[i] = environment["HOME"] + paths[i].substr(1);
        }
        if (i == 0){
            pathString = paths[i];
        }else{
            pathString = pathString + ":" + paths[i];
        }
    }
    environment["PATH"] = pathString;
}

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
        return 1;
		}
	else {
        printf("%sError:%s Directory not found.\n", RED, RESET);
        return 1;
	}
}

int handleCDHome() {
    chdir(environment["HOME"].c_str());
    return 1;
}

int handleCDTilde(string user) {
    struct passwd* pwd;
    pwd = getpwnam(user.c_str());
    if (pwd) {
        environment["PROMPT"] = pwd->pw_dir;
        chdir(pwd->pw_dir);
    } else {
        printf("%sError:%s User not found.\n", RED, RESET);
    }

    return 1;
}

int handleSETENV(string name, string value){
    if ((name.length() > 0) && (value.length() > 0)){
        environment[name] = value;
        if (name == "PATH"){
            expandPathEnv();
        }
        return 1;
    }else{
        printf("%sError:%s Invalid input.", RED, RESET);
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
        printf("%sError:%s Invalid input.", RED, RESET);
        return 1;
    }
}

int handleSetAlias(string name, string value){
    if (name == value){
        printf("%sError:%s Expansion of \"%s\" would create a loop.\n", RED, RESET, name.c_str());
        return 1;
    }
    for (auto i = aliases.begin(); i != aliases.end(); i++){
        if (((i->first).c_str() == name) && ((i->first).c_str() == name)){
            printf("%sError:%s Expansion of \"%s\" would create a loop.\n", RED, RESET, name.c_str());
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
    printf("%sError:%s %s alias not found.", RED, RESET, name.c_str());
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

int handleCommand(string name, vector<string> args, bool pipeIn, bool pipeOut, string inputFile, string outputFile, bool append){
    pid_t p1 = fork(), parentpid;
    int status = 0;
    int execRes = 0;

    char **argArray;
    int argArrayLength = 0;

    vector<string> paths;

    argArrayLength = 0;

    // Create C array version of arg vector
    argArray = (char**) malloc((args.size() + 2) * sizeof(char*));

    // Add the command name as the first element
    argArray[0] = (char *) malloc((name.length()) * sizeof(char));
    strcpy(argArray[0], name.c_str());
    argArrayLength++;

    // Loop through args and add them to argArray
    for (int i = 1; i < (args.size() + 1); i++){
        argArray[i] = (char *) malloc((args[i - 1].length()) * sizeof(char));
        strcpy(argArray[i], args[i - 1].c_str());
        argArrayLength++;
    }

    // Add in the null element to the end of the array
    argArray[args.size() + 1] = NULL;
    
    // Split up paths to know where to find command executables
    tokenize(environment["PATH"], ':', paths);

    if(p1 < 0)
    {
        printf("%sError:%s Process could not be created to run system command %s.", RED, RESET, name.c_str());
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
    }
    else
    {
        int inTempPipe = 0, outTempPipe = 0;
        string writePermission;
        string tempInput = inputFile;
        string tempOutput = outputFile;

        if (inputFile.length() == 0){
            tempInput = "tempPipe";
        }
        if (outputFile.length() == 0){
            tempOutput = "tempPipe";
            writePermission = "w";
        }else{
            writePermission = append ? "a" : "w";
        }
        
        if (pipeIn && pipeOut){
            
            if (tempInput == tempOutput){
                FILE* inTempFile = fopen(tempInput.c_str(), ("r" + writePermission).c_str());
                inTempPipe = fileno(inTempFile);
                dup2(inTempPipe, 0);
                dup2(inTempPipe, 1);
                fclose(inTempFile);
            }else{
                FILE* inTempFile = fopen(tempInput.c_str(), "r");
                FILE* outTempFile = fopen(tempOutput.c_str(), writePermission.c_str());
                inTempPipe = fileno(inTempFile);
                outTempPipe = fileno(outTempFile);
                dup2(inTempPipe, 0);
                dup2(outTempPipe, 1);
                fclose(inTempFile);
                fclose(outTempFile);
            }
        } else if (pipeIn){
            FILE* tempFile = fopen(tempInput.c_str(), "r");        
            inTempPipe = fileno(tempFile);
            dup2(inTempPipe, 0);
            fclose(tempFile);
        } else if (pipeOut){
            FILE* tempFile = fopen(tempOutput.c_str(), writePermission.c_str());
            outTempPipe = fileno(tempFile);
            dup2(outTempPipe, 1);
            fclose(tempFile);
        }
        for (int i = 0; i < paths.size(); i++){
            execRes = execv((paths[i] + "/" + name).c_str(), argArray);
        }
        printf("%sError:%s Command %s not found.\n", RED, RESET, name.c_str());
        exit(0);
    }   
    return 1;
}

int handleCommandTable(){
    // for (int i = 0; i < cmdTable.size(); i++){
    //     printf("Command %s, in: %s, out: %s.\n", cmdTable[i].name.c_str(), cmdTable[i].in.c_str(), cmdTable[i].out.c_str());
    // }
    if (cmdTable.size() == 1){
        handleCommand(cmdTable[0].name, cmdTable[0].args, cmdTable[0].in != "", cmdTable[0].out != "", cmdTable[0].in, cmdTable[0].out, cmdTable[0].appendOutput);
        cmdTable.clear();
        return 1;
    }
    for (int i = 0; i < cmdTable.size(); i++){
        if (((i - 1) >= 0) && ((i + 1) < cmdTable.size())){ // Middle
            handleCommand(cmdTable[i].name, cmdTable[i].args, true, true, "", "", cmdTable[i].appendOutput);
        }else if((i - 1) >= 0){ // End
            handleCommand(cmdTable[i].name, cmdTable[i].args, true, cmdTable[i].out.length() != 0, "", cmdTable[i].out, cmdTable[i].appendOutput);
        }else if((i + 1) < cmdTable.size()){ // Beginning
            handleCommand(cmdTable[i].name, cmdTable[i].args, cmdTable[i].in.length() != 0, true, cmdTable[i].in, "", cmdTable[i].appendOutput);
        }
    }
    remove("tempPipe");
    cmdTable.clear();
    return 1;
}

int handleArgs(string word){
    curArgs.push_back(word);
    return 1;
}

int handleCommands(string word){
    CommandInfo tempCmdInfo;
    tempCmdInfo.name = word;
    tempCmdInfo.args = curArgs;
    cmdTable.push_back(tempCmdInfo);
    curArgs.clear();
    return 1;
}

int handleInRed(string name){
    cmdTable[0].in = name;
    return 1;
}

int handleOutRed(string name, bool append){
    cmdTable[cmdTable.size() - 1].out = name;
    cmdTable[cmdTable.size() - 1].appendOutput = append;
    return 1;
}

int main()
{
    getcwd(cwd, sizeof(cwd));

    vector<string> dirs;
    tokenize(cwd, '/', dirs);

    environment["PWD"] = cwd;
    environment["HOME"] = getenv("HOME");
    environment["PROMPT"] = dirs[dirs.size() - 1];
    environment["PATH"] = getenv("PATH");
    expandPathEnv();

    system("clear");
    while(1)
    {
        getcwd(cwd, sizeof(cwd));
        environment["PWD"] = cwd;
        
        dirs.clear();
        tokenize(cwd, '/', dirs);
        environment["PROMPT"] = dirs[dirs.size() - 1];
        printf("%snUtShElL ❖%s %s%s ➤➤ %s", GREEN, RESET, BOLDCYAN, environment["PROMPT"].c_str(), RESET);
        yyparse();
    }

   return 0;
}