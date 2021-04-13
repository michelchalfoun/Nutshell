#include "../command_handling.h"
#include "../helpers.h"

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