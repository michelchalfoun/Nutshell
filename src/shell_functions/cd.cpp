#include "../command_handling.h"

int handleCD(string newDir){
    vector<string> dirs;
    if ((newDir.find("*") == string::npos) && (newDir.find("?") == string::npos)){
        dirs.push_back(newDir);
    }else{
        dirs = getWildcardArgs(newDir);
    }
    
    if (dirs.size() > 1){
        string error = "Too many directories";
        yyerror((char *)error.c_str());
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
        string error = "Directory not found";
        yyerror((char *)error.c_str());
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
        string error = "User not found";
        yyerror((char *)error.c_str());
    }

    return 1;
}