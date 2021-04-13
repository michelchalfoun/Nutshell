#include "../command_handling.h"

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