#include "command_handling.h"

using namespace std;

map<string, string> environment;
map<string, string> aliases;

bool outputRedirectionBuiltIn;
string outputRedirectionBuiltInFilename;
bool outputRedirectionBuiltInAppend;

// bool errRedirectionBuiltIn;
// string errRedirectionBuiltInFile;

vector<string> curArgs;
bool backgroundEnable = false;
vector<CommandInfo> cmdTable;

char *getcwd(char *buf, size_t size);
int yyparse();

int main()
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    vector<string> dirs;
    splitString(cwd, '/', dirs);

    environment["PWD"] = cwd;
    environment["HOME"] = cwd;
    environment["PROMPT"] = dirs[dirs.size() - 1];
    // environment["PATH"] = "."; //Default
    environment["PATH"] = ".:/bin:/usr/bin:/usr/local/bin";
    expandPathEnv();

    // system("clear");
    while(1)
    {
        getcwd(cwd, sizeof(cwd));
        environment["PWD"] = cwd;
        
        dirs.clear();
        splitString(cwd, '/', dirs);
        environment["PROMPT"] = dirs[dirs.size() - 1];
        printf("%snUtShElL ❖%s %s%s ➤➤ %s", GREEN, RESET, BOLDCYAN, environment["PROMPT"].c_str(), RESET);
        yyparse();
    }

   return 0;
}