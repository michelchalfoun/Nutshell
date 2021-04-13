

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
#include <glob.h>
#include <string.h>

using namespace std;

char *getcwd(char *buf, size_t size);
int yyparse();

map<string, string> environment;
map<string, string> aliases;

vector<string> curArgs;
bool backgroundEnable = false;
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
    vector<string> dirs = getWildcardArgs(newDir);
    if (dirs.size() > 1){
        printf("%sError:%s Too many directories.\n", RED, RESET);
        return 1;
    }
    char arr[FILENAME_MAX];
    string curDir = getcwd(arr, sizeof(arr));
    string targetDir = curDir + "/" + dirs[0];

    if (dirs[0][0] == '/') { // arg is relative path
		targetDir = dirs[0];
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
        printf("%sError:%s Invalid input.\n", RED, RESET);
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
    if (name == "PATH" || name == "HOME"){
        printf("%sError:%s Cannot unset %s.\n", RED, RESET, name.c_str());
        return 1;
    }
    if (name.length() > 0){
        environment.erase(name);
        return 1;
    }else{
        printf("%sError:%s Invalid input.\n", RED, RESET);
        return 1;
    }
}

int handleSetAlias(string name, string value){
    if (name == value){
        printf("%sError:%s Expansion of \"%s\" would create a loop.\n", RED, RESET, name.c_str());
        return 1;
    }
    // Infinite loop check
    auto foundAlias = aliases.find(value);
    while(foundAlias != aliases.end()){
        if (foundAlias->second == name){
            printf("%sError:%s Expansion of \"%s\" would create a loop.\n", RED, RESET, name.c_str());
            return 1;
        }
        foundAlias = aliases.find(foundAlias->second);
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
    auto foundAlias = aliases.find(name);
    while (aliases.find(foundAlias->second) != aliases.end()){
        foundAlias = aliases.find(foundAlias->second);
    }
    return (foundAlias->second).c_str();
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

int handleCommand(string name, vector<string> args, bool pipeIn, bool pipeOut, string inputFile, string outputFile, bool append, string errOutput){
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
        
        if (errOutput.length() != 0){
            if (errOutput == "&1"){
                dup2(1, 2);
            }else{
                FILE* errTempFile = fopen(errOutput.c_str(), "w");
                int errTempPipe = fileno(errTempFile);
                dup2(errTempPipe, 2);
                fclose(errTempFile);
            }
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
        handleCommand(cmdTable[0].name, cmdTable[0].args, cmdTable[0].in != "", cmdTable[0].out != "", cmdTable[0].in, cmdTable[0].out, cmdTable[0].appendOutput, cmdTable[0].errOutput);
        // cmdTable.clear();
        // return 1;
    }else{
        for (int i = 0; i < cmdTable.size(); i++){
            if (((i - 1) >= 0) && ((i + 1) < cmdTable.size())){ // Middle
                handleCommand(cmdTable[i].name, cmdTable[i].args, true, true, "", "", cmdTable[i].appendOutput, cmdTable[i].errOutput);
            }else if((i - 1) >= 0){ // End
                handleCommand(cmdTable[i].name, cmdTable[i].args, true, cmdTable[i].out.length() != 0, "", cmdTable[i].out, cmdTable[i].appendOutput, cmdTable[i].errOutput);
            }else if((i + 1) < cmdTable.size()){ // Beginning
                handleCommand(cmdTable[i].name, cmdTable[i].args, cmdTable[i].in.length() != 0, true, cmdTable[i].in, "", cmdTable[i].appendOutput, cmdTable[i].errOutput);
            }
        }
    }
    remove("tempPipe");
    cmdTable.clear();
    backgroundEnable = false;
    return 1;
}

int handleCommandTableBG(){
    // int backgroundStatus = 0;
    // printf("bg: %d\n", backgroundEnable);

    pid_t backgroundProcess = fork();
    
    if(backgroundProcess < 0)
    {
        printf("%sError:%s Process could not be created to run system command table.", RED, RESET);
    }
    else if(backgroundProcess == 0)
    { 
        
        handleCommandTable();

        printf("\n[1] %d is done.\n", backgroundProcess);
        // fflush(nullptr);
        // exit(0);
        // return 1;
    }else
    {
        printf("[1] %d\n", backgroundProcess);
        // fflush(nullptr);
        // return 1;
    }
    return 1;
}

int handleArgs(string word){
    vector<string> filenames = getWildcardArgs(word);
    for (int i = 0; i < filenames.size(); i++){
        curArgs.push_back(filenames[i]);
    }
    
    return 1;
}

vector<string> getWildcardArgs(string arg){
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));
    int return_value = glob(arg.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value != 0) {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        printf("Error: %s", ss.str().c_str());
    }
    vector<string> filenames;
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return filenames;
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

int handleErrRed(string name, bool stdOutput){
    if (stdOutput){
        cmdTable[cmdTable.size() - 1].errOutput = "&1";
    }else{
        cmdTable[cmdTable.size() - 1].errOutput = name;
    }
    return 1;
}

int enableAmpersand(){
    backgroundEnable = true;
    return 1;
}

int main()
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    vector<string> dirs;
    tokenize(cwd, '/', dirs);

    environment["PWD"] = cwd;
    environment["HOME"] = cwd;
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