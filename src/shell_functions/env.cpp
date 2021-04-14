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