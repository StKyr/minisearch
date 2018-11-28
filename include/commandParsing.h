#ifndef MINISEARCH_COMMANDPARSING_H
#define MINISEARCH_COMMANDPARSING_H


/**
 * A namespace to be used for checking the type of each command
 */
enum commandCodes {_INVALID_, SRCH, DF, TF, EXT, BLNK};


/**
 * A structuring holding information about the command the user input.
 * It is meant to be updated by `parseCommand()` and then accessed by main.
 * No need for abstraction here, it is just a collection of data main() needs to know
 */
typedef struct cmd {

    enum commandCodes type; /* Its type. Client code should check this to determine appropriate action */
    int paramId;            /* the document id TF command needs. In other cases it should be ignored */
    int numOfWords;         /* number of terms in a SRCH (1,...,10) , DF (0,1) or TF (1) query */
    char *words[10];        /* the parameter terms of the command. Their actual number is stored in `numOfWords`.*/

} Command;


/**
 * Reads a single line of code, determines if it is a valid command
 * and if so updates `command` members accordingly.
 * @param command The command in which to store info. Namely, its type is set as an appropriate value from
 * `enum commandCodes` and if it was a valid command it also sets `numOfWords` and members of `words` attributes.
 * If it was not a valid command, `command.Type` is set to `_INVALID_`.
 * @return 0 if a valid command was parsed, non-zero otherwise
 */
int parseCommand(Command *command);


#endif //MINISEARCH_COMMANDPARSING_H
