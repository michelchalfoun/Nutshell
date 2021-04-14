#include "helpers.h"

using namespace std;

void splitString(string const &str, const char delim, vector<string> &out)
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
    splitString(environment["PATH"], ':', paths);
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

vector<string> getWildcardArgs(string arg){
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));
    int return_value = glob(arg.c_str(), GLOB_TILDE, NULL, &glob_result);
    vector<string> filenames;
    if(return_value != 0) {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        printf("Error: %s", ss.str().c_str());
        return filenames;
    }
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return filenames;
}

