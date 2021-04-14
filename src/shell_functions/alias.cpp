#include "../command_handling.h"

int handleSetAlias(string name, string value){
    if (name == value){
        // printf("%sError:%s Expansion of \"%s\" would create a loop.\n", RED, RESET, name.c_str());
        string error = "Expansion of " + name + " would create a loop";
        yyerror((char *)error.c_str());
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
    for (auto i = aliases.begin(); i != aliases.end(); i++){
        printf("%s=%s\n", (i->first).c_str(),(i->second).c_str());
    }
    dup2(saved_stdout, 1);
    close(saved_stdout);
    outputRedirectionBuiltIn = false;
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