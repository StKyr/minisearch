#include <stdio.h>                     /* for fprintf, stderr */
#include <stdbool.h>                   /* for true */
#include <stdlib.h>                    /* for malloc, free, strtoll, EXIT_SUCCESS, EXIT_FAILURE */
#include <getopt.h>                    /* for optopt, opterr, getopt_long, required_argument, optional_argument */
#include <memory.h>                    /* for strlen, strcpy */
#include <ctype.h>                     /* for isprint */

#include "../include/commandParsing.h" /* for Command, parseCommand, commandCodes enum */
#include "../include/index.h"          /* for Index, buildIndex, Index_Destroy */
#include "../include/search.h"         /* for searchDocumentFrequency, searchTermFrequency, searchDocuments */


/**
 * A literal struct declaration containing command line parameters of the program.
 * Parameters should be global in the scope of the main module.
 */
struct {
    /* The name (and path) of the input file. Should be free'd afterwards */
    char *docfile;

    /* How many documents a query should display (optional, default: 10)*/
    int K;

} Params;



void parseCommandLineArguments(int argc, char *argv[]);
void cleanUP(Index **pIndex, Command command);


int main(int argc, char *argv[]) {

    //testArray();
    //return 0;




    int i;                                                   // general purpose counter

    parseCommandLineArguments(argc, argv);                   // parse arguments and update global `Params` variable

    Command command;                                         // Stores information about what the user typed as input
    for (i=0; i<10; i++)command.words[i] = NULL;
    command.numOfWords = 0;

    Index *index;                                            // The only object needed on top level.
                                                             // Contains both the Map and the Inverted Index.
    if (buildIndex(&index, Params.docfile)){                 // parse input file to initialize structures
                                                             // unsuccessful execution due to docfile errors
        fprintf(stderr, "Input file corrupt or non existing.\n");
        cleanUP(&index, command);
        return EXIT_FAILURE;
    }




    /*
    if (parseCommand(&command)){

        fprintf(stderr, "Malformed command.\n");
        for (i=0; i<command.numOfWords; i++) free(command.words[i]);
        //if (command.words) free(command.words);
        Index_Destroy(&index);
        free(Params.docfile);
        exit(EXIT_FAILURE);
    }
     */

    parseCommand(&command);                                  // get initial input
    while (true){                                            // loop here to process user input and perform action
        char *term;
        switch(command.type){                                // select action by Type (enum `commandCodes`)

            case SRCH:                                       // input is "/search word1 word2 ..."
                searchDocuments(index, command.words, command.numOfWords, Params.K);
                break;

            case DF:                                         // input is "/df" or "/df term"
                term = (command.numOfWords != 0) ? command.words[0] : NULL;
                searchDocumentFrequency(index, term);
                break;

            case TF:                                         // input is "/tf id term"
                searchTermFrequency(index, command.paramId, command.words[0]);
                break;

            case EXT:                                        // input is "/exit"
                cleanUP(&index, command);
                exit(EXIT_SUCCESS);
                break;

            case BLNK:
                break;

            case _INVALID_:                                  //invalid command or incorrect syntax
            default:
                fprintf(stderr,"Unrecognised command or malformed syntax.\n");
                cleanUP(&index, command);
                exit(EXIT_FAILURE);
        }
        parseCommand(&command);
        /*
        if (parseCommand(&command)){
            fprintf(stderr, "Invalid or malformed command.\n");
            for (i=0; i<command.numOfWords; i++) free(command.words[i]);
            //if (command.words) free(command.words);
            Index_Destroy(&index);
            free(Params.docfile);
            exit(EXIT_FAILURE);
        }
         */
    }
    /*
    // never reach here
    int i;
    for (i=0; i<command.numOfWords; i++) free(command.words[i]);
    if (command.words) free(command.words);
    Index_Destory(&index);
     free(Params.docfile);
    return EXIT_FAILURE;
    */
}



/**
 * Parse atguments in argv and store them in `Params` global struct using `getopt`.
 * Incase of invalid arguments this function will terminate execution by itself.
 * @param argc
 * @param argv
 */
void parseCommandLineArguments(int argc, char* argv[]){
    Params.K = 10;                                           // default value
    Params.docfile = NULL;

                                                             // standard getopt parsing. Not much to comment here.
                                                             // we also accept long argument --input and --number
    int c;
    opterr = 0;
    int option_index = 0;
    static struct option long_options[] = {
                    {"input",       required_argument, 0,           'i'},
                    {"number",    optional_argument, 0,           'k'},
                    {0, 0, 0, 0}
            };
    while ((c = getopt_long(argc, argv, "k:i:", long_options, &option_index)) != -1)
        switch (c) {
            case 'i':
                Params.docfile = malloc((1 + strlen(optarg))* sizeof(char));
                strcpy(Params.docfile, optarg);
                break;
            case 'k':
                Params.K = (int)strtoll(optarg,NULL,10);
                if (Params.K <= 0){
                    fprintf(stderr, "Invalid value %d for argument k.\n", Params.K);
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                if (optopt == 'i' || optopt == 'k') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if (isprint (optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                free(Params.docfile);
                exit(EXIT_FAILURE);
                break;
            default:
                free(Params.docfile);
                break;
        }

    if (Params.docfile == NULL){                             // no -i argument
        fprintf(stderr, "Error: Required -i argument.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Do all memory cleanup here
 * @param pIndex pointer to Index type for freeing
 * @param command The command we use (have to cleanup its words which are malloc'd strings)
 */
void cleanUP(Index **pIndex, Command command){
    Index_Destroy(pIndex);
    int i;
    for (i=0; i<10; i++) free(command.words[i]);
    free(Params.docfile);
}




//-----------------------------------------------------------------------------------------




void testArray() {

    bool found = false;
    int i, j;

    int K = 5;

    int num = 100;

    int A[K];
    A[0] = 50;
    A[1] = 40;
    A[2] = 30;
    A[3] = 20;
    A[4] = 10;


    for (i=0; i<K; i++){
        if (num >= A[i]){
            found = true;
            break;
        }
    }

    if (found) {

        for (j = K - 1; j > i; j--) {
            A[j] = A[j - 1];
        }
        A[i] = num;
    }
    /*
    for (i=0; i<K; i++){

        if (num <= A[i]){
            B[i] = A[i];
        }else{
             if (!found){
                 B[i] = num;
                 found = true;
                 if (i<K-1) {
                     B[i + 1] = A[i];
                 }
             }else{
                 B[i] = A[i];
             }


        }



    }
    */


    for (i=0; i<K; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");
}