#include "../command_handling.h"

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
    int saved_stdout;
    if (outputRedirectionBuiltIn){
        saved_stdout = dup(1);
        string writePermission = "w";
        if (outputRedirectionBuiltInAppend){
            writePermission = "a";
        }
        FILE* tempFile = fopen(outputRedirectionBuiltInFilename.c_str(), writePermission.c_str());
        int outTempPipe = fileno(tempFile);
        dup2(outTempPipe, 1);
    }
    for (auto i = environment.begin(); i != environment.end(); i++){
        printf("%s:%s\n", (i->first).c_str(), (i->second).c_str());
    }
    dup2(saved_stdout, 1);
    close(saved_stdout);
    outputRedirectionBuiltIn = false;
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