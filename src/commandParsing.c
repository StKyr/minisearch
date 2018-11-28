#include <stdio.h>                                        /* for getline, stdin */
#include <memory.h>                                       /* for strtok, strlen, strcmp */
#include <stdlib.h>                                       /* for malloc, free, strtoll */
#include <stdbool.h>                                      /* for bool, true, false */
#include "../include/commandParsing.h"

bool isNumOfParametersCorrect(Command *command, int numOfParameters);
bool parseCommandType(Command *command, char *firstpart);



/**
 * Reads a single line of code, determines if it is a valid command
 * and if so updates `command` members accordingly.
 * This is the only function with global scope to be used by main code
 * @param command The command in which to store info. Namely, its type is set as an appropriate value from
 * `enum commandCodes` and if it was a valid command it also sets `numOfWords` and members of `words` attributes.
 * If it was not a valid command, `command.Type` is set to `_INVALID_`.
 * @return 0 if a valid command was parsed, non-zero otherwise
 */
int parseCommand(Command *command){

    int i;                                                           // cleaning up memory from previous execution
    for (i=0; i< command->numOfWords; i++) free(command->words[i]);
    for (i=0; i<10; i++) command->words[i] = NULL;



    char *line = NULL;                                               // buffer to store input line
    unsigned long inputSize=0;                                       // setting its size 0 and line to NULL
                                                                     // will make getline to allocate space
    getline(&line, &inputSize, stdin);                               // read input line

    if (strlen(line) ==1){                                           // a single newline character
        command->type = BLNK;
        free(line);
        return 0;
    }

    char *firstpart, *part;                                          // need them for strtok calls
    firstpart = strtok(line, " \t\n");                               // store the first word to determine command type

    if (!firstpart){                                                 // empty line
        free(line);
        return -5;
    }

    if (! parseCommandType(command, firstpart)){                     // find what command it is by its first word
                                                                     // invalid first word
        free(line);
        return -1;
    }


    int numOfParameters = 0;
    while ((part = strtok(NULL, " \t\n")) != NULL){                  // parse command parameters one by one
        if (++numOfParameters > 10) break;                           // more than 10 words, ignore the rest

        if (numOfParameters == 1 && command->type == TF){            // if we are dealing with a TF command
            command->paramId = (int) strtoll(part, NULL, 10);        // first word should be the ID (int)

        }else{
                                                                     // normalize term index
                                                                     // cause TF needs int as first arg so not a word
            int wordIndex = (command->type == TF) ? numOfParameters-2 : numOfParameters-1;

            command->words[wordIndex] = malloc((1+strlen(part))*sizeof(char));
            strcpy(command->words[wordIndex], part);                 // store that word in command
        }

    }
    free(line);

    if (numOfParameters > 10) numOfParameters--;                     // if words are more than 10 its value will be 11
    command->numOfWords = (command->type == TF) ? numOfParameters -1 : numOfParameters; // the same issue with TF

    if (! isNumOfParametersCorrect(command, numOfParameters)){
                                                                     // malformed command
        return -2;
    }

    return 0;
}


/**
 * Use string comparisons to determine the type of each command
 * @param command The command to update its `Type`
 * @param firstpart the first word of the input
 * @return `true` if it was a valid command type, `false` otherwise
 */
bool parseCommandType(Command *command, char *firstpart){
    if (!strcmp(firstpart, "/search")){
        command->type = SRCH;

    }else if (!strcmp(firstpart, "/df")){
        command->type = DF;

    }else if (!strcmp(firstpart, "/tf")){
        command->type = TF;

    }else if (!strcmp(firstpart, "/exit")){
        command->type = EXT;

    }else{
        command->type = _INVALID_;
        return false;
    }

    return true;
}


/**
 * Check if the command has appropriate parameters. Namely:
 * - SRCH command should have at least one and at most 10 words
 * - DF command should have 0 or 1 word
 * - TF command should have an ID (int) param and a word
 * - EXT command should not have any words
 * @param command an already parsed command
 * @param numOfParameters how many parameters we encountered
 * @return `true` if the command meets the above criteria, `false` otherwise
 */
bool isNumOfParametersCorrect(Command *command, int numOfParameters){
    if (command->type == SRCH && numOfParameters == 0){
        return false;
    }

    if (command->type == EXT && numOfParameters != 0){
        return false;
    }

    if (command->type == TF && numOfParameters != 2){
        return false;
    }

    if (command->type == DF && numOfParameters > 1){
        return false;
    }

    return true;
}
