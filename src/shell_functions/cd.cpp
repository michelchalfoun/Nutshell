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

char* autofill(char* partialDir){
    string fixedString = (string) partialDir + "*";
    vector<string> dirs = getWildcardArgs(fixedString.c_str());
    return (char *) dirs[0].c_str();
}

char* autofillTilde(char* partialDir){
    vector<string> users;
    while (true) {
        passwd* entry = getpwent();
        if (!entry) {
            break;
        }
        if (entry->pw_name[0] == '_'){
        }else if (strcmp(entry->pw_name, "root") == 0){
            break;
        }else{
            users.push_back(entry->pw_name);
        }
    }
    endpwent();
    users.erase(users.begin());
    vector<string> newUsers;
    for (int i = 0; i < users.size(); i++){
        if (users[i].rfind(partialDir, 0) == 0) {
            newUsers.push_back(users[i]);
        }
    }
    if (newUsers.size() > 0){
        char* userName = getpwnam(newUsers[0].c_str())->pw_dir;
        return userName;
    }
    return nullptr;
}