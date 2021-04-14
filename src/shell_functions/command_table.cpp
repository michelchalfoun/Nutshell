#include "../command_handling.h"

int handleArgs(string word){
    if (word.find('*') != string::npos || word.find('?') != string::npos){
        vector<string> filenames = getWildcardArgs(word);
        for (int i = 0; i < filenames.size(); i++){
            curArgs.push_back(filenames[i]);
        }
    }else{
        curArgs.push_back(word);
    }
    
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