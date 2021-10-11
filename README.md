# Nutshell Project - OS
#### _Michel El Chalfoun and Masen Beliveau_
This README is for the Nutshell Project for COP 4600 - Spring 2021.
***
## Running the Nutshell
Run `make` in the terminal, making sure you're inside the `Nutshell` directory where the Makefile resides. Then run `./nutshell` to start up the Nutshell. If you would like to exit out of the Nutshell simply run the `bye` command.
**PS:** When it comes to the features that involve different users, our computers sometimes required admin permissions (`sudo`) when executing. This is highly dependent on the computer on which the Nutshell is running on. However, this can be circumvented by running `sudo ./nutshell`, giving all admin permissions to the Nutshell.
***
## Features implemented
Here is a list of features we were able to implement:
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
