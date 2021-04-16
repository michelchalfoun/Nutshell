# Nutshell Project - OS
#### _Michel El Chalfoun and Masen Beliveau_
This README is for the Nutshell Project for COP 4600 - Spring 2021.
***
## Running the Nutshell
Run `make` in the terminal, making sure you're inside the `Nutshell` directory where the Makefile resides. Then run `./nutshell` to start up the Nutshell. If you would like to exit out of the Nutshell simply run the `bye` command.
**PS:** When it comes to the features that involve different users, our computers sometimes required admin permissions (`sudo`) when executing. This is highly dependent on the computer on which t
***
## Contributions
##### Michel El Chalfoun
Worked on developing the parser logic in lex, other (system) commands handling with I/O redirection for input output and error, piping, wildcard matching, and the extra credit sections.
Files: command_table.cpp, commands.cpp, io_redirection.cpp, nutshell.cpp, global.h, nutshparser.l, nutshparser.y, Makefile, README

##### Masen Beliveau
Worked on coding built-in commands section, mainly implementing env (set, get, print), HOME and PATH variables, cd word, and alias. Also implemented yacc logic. 
Files: alias.cpp, cd.cpp, env.cpp, nutshparser.l, nutshparser.y, helpers.cpp, exit.cpp, background.cpp, README

Both Michel and Masen worked on error handling, where Michel focused on handling errors before they happened, and Masen focusing on running through the sample test commands and debugging any errors that arised, while both still overlapped across the two.
***
## Features not implemented
We have all the features implemented.
***
## Features implemented
Here is a list of features we weren't able to implement:
- Parsing and Characters
  - Handles words, white space, dots, tildes and metacharacters
- Built-in Commands
  - setenv variable word - unsetenv variable - printenv
  - HOME and PATH environment variables
  - cd word
  - alias - unalias name - alias name word
  - bye
- Other Commands/Features
    - I/O redirection (Input, Output (Overwrite and Append), Error)
    - Piping
    - Background process
    - Wildcard matching
- Extra Credit
    - Tilde Expansion (Standalone and Username)
    - File Name Completion (Filenames, directories and users)
***
