#include "../command_handling.h"

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