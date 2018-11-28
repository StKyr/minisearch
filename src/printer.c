
#include <stropts.h>
#include <sys/ioctl.h>
#include <zconf.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/printer.h"

#define TABLNGTH 8


/**
 * Same as strlen but also considers UTF-8 characters
 * @param s The string whose length should be counted
 * @return How many (UTF-8) characters are there
 */
size_t utf8len(char *s)
{
    size_t len = 0;
    for (; *s; ++s) if ((*s & 0xC0) != 0x80) ++len;
    return len;
}


/**
 * Check if a given substring appearing in a text consists a whole word.
 * Namely it checks whether it is proceeded and followed by a whitespace character
 * or is either at the beginning or the end of the text.
 * @param substr
 * @param text
 * @return
 */
bool isWholeWord(char *substr, char *text){
    return
            ( &(substr[0]) == &(text[0]) || isspace((substr-1)[0]) )
            &&
            ( &(substr[strlen(substr)-1]) == &(text[strlen(text)-1]) || isspace((substr+strlen(substr))[0]) )
    ;
}

/**
 * Get the size of the terminal screen in columns. If not able set default to 80 and print warning.
 * @return the number of columsn the terminal is consisted of.
 */
int getTerminalColumns(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_col){
        return w.ws_col;
    }else{
        return 80;
    }
}

/**
 * Compute how many decimal digits a number has.
 * Same as the ceiling function of its base 10 logarithm, but more accurate.
 * @param number The number to count its digits.
 * @return the number of digits this number has.
 */
int digitsOf(int number){
    if (number<0) number = -number;
    int d=1;
    while (number/10 > 0){
        d++;
        number /= 10;
    }
    return d;
}


/**
 * Calculate how many characters initial offset printing will occupy.
 * @param score The score of a certain document.
 * @param maxId The largest ID to be printed
 * @param K the number of documents to be printed
 * @return how many characters will be printed for this offset.
 */
int getOffsetSize(double score, int maxId, int K){
    int lineNumbering = digitsOf(K);
    int dotOffset = 1;
    int parenthesisOffset = 2;
    int idOffset = digitsOf(maxId);
    int bracketsOffset = 2;
    int scoreOffset = digitsOf((int)score) + 4 + 1;
    int signOffset = (score<0) ? 1 : 0;
    int spaceOffset = 1;

    return lineNumbering
           +dotOffset
           +parenthesisOffset
           +idOffset
           +bracketsOffset
           +scoreOffset
           +signOffset
           +spaceOffset;
}



/**
 * Write to a string the initial offset substring for a certain document.
 * @param str The string write on.
 * @param id The id of the document to be printed
 * @param lineNumber Its line numbering (0 based)
 * @param score The score of the document
 * @param maxId The largest ID which will be printed
 * @param K how many documents are going to be printed
 */
void formatInitial(char *str, int id, int lineNumber, double score, int maxId, int K){
    int i;
    int numBeginningSpaces = digitsOf(K)-digitsOf(lineNumber);
    int current = 0;

    for (i=0; i<numBeginningSpaces; i++){              // print line numbering offset in case documents are more than 10
        str[i] = ' ';
    }

    current = numBeginningSpaces;
    snprintf(str+current, (size_t)digitsOf(lineNumber)+1,"%d",lineNumber); // print its line numbering

    current = numBeginningSpaces+digitsOf(lineNumber);
    str[current] = '.';


    current++;                                                            // start formatting its ID
    str[current] = '(';

    current++;
    int numIdSpaces = digitsOf(maxId) - digitsOf(id);
    for (i=0; i<numIdSpaces; i++){                                       // print offset for its ID
        str[current+i] = ' ';
    }

    current += numIdSpaces;
    snprintf(str+current, (size_t)digitsOf(id)+1,"%d",id);              // print the ID

    current += digitsOf(id);

    str[current] = ')';
    current++;

    int scoreSize = digitsOf((int)score) + 4;                          // print its score
    if (score >= 0) {
        snprintf(str + current, (size_t) scoreSize + 2 + 1 + 1 + 1, "[%.4f] ", score);
    }else{                                                             // negative scores occupy one more place
        snprintf(str + current, (size_t) scoreSize + 2 + 1 + 1 + 1+1, "[%.4f] ", score);
    }
}




/**
 * given an output text substitute all its TAB characters with the appropriate amount of SPACES
 * such as when printed they will have the same result
 * @param text The original text containing TABS
 * @param newText the new text with the tabs substitute
 * @param offset The offset for printing initial info of the first line
 * @param totalLineWidth how many characters a line has
 */
void substituteTabs(const char *text, char *newText, int offset, int totalLineWidth){

    int i;
    int charsInserted=0;                                       // how many characters have been inserted in the new text
    int charsInLine=offset;                                    // how many characters are so far in the current line
    char ch;

    for (i=0; i<strlen(text); i++){

        if ((ch = text[i]) != '\t'){                           // a normal character
            newText[charsInserted++] = ch;                     // just insert him to new string
            charsInLine++;
            if (charsInLine == totalLineWidth){                // end of line here
                charsInLine = offset;                          // we have already "placed" `offset` characters there.
            }
        }else{                                                 // we need to manually substitute TAB character
            int nSpaces = TABLNGTH - charsInLine % TABLNGTH;   // how many spaces this TAB will actually print

            int j;
            for (j=0; j<nSpaces; j++){
                if (charsInLine + j == totalLineWidth) break;  // if we reach end of line stop inserted spaces

                newText[charsInserted+j] = ' ';                // insert each space in each place
            }

            charsInserted += j;                                // notify accordingly
            charsInLine += j;
            if (charsInLine == totalLineWidth){                // check for end of line
                charsInLine = offset;
            }
        }
    }
    newText[charsInserted]='\0';
}



/**
 * Prepare the string of a single line to be printed for underlining such as in any posiytion
 * a term appears in the original text, the '^' character is displayed, otherwise
 * a space charcter is placed.
 * @param underliningString The string on which to format the underlining characters
 * @param text The whole original text
 * @param startIndex The index of the original text from which the next line will start
 * @param lineWidth Number of characters per line
 * @param terms An array of the terms to be underlined
 * @param numTermsHow many terms are there
 * @param carriage How many characters should be underlined at the beginning due to word splitting
 * at the end of the previous line. It will also be updated in the current call.
 */
void underline(char *underliningString, char *text, int startIndex, int lineWidth, char **terms, int numTerms, int *carriage){

    memset(underliningString, ' ', (size_t)lineWidth);                       // initially put spaces everywhere
    underliningString[lineWidth]='\0';

    int i;
    for (i=0; i<*carriage; i++){                                             // underline word the split in pevious line
        underliningString[i] = '^';
    }
    *carriage = 0;                                                           // initialize value for current carriage
    int j;
    for(j=0; j<numTerms; j++) {                                              // for each word

        char *currPart = &(text[startIndex]);                                // start of line on text
        char *substr;
        while ((substr = strstr(currPart, terms[j])) != NULL) {              // if term is found

            if (substr - &(text[startIndex]) > lineWidth) break;             // if it is inside the "line" limits

            if (!isWholeWord(substr, text)) {                                // the word should not be a "sub-word"
                currPart = substr + utf8len(terms[j]) + 1;
                continue;
            }

            int pos = (int) (substr - &text[startIndex]);                    // where is that word located
            memset(underliningString + pos, '^', utf8len(terms[j]));         // underline here
            currPart = substr + utf8len(terms[j]) + 1;                       // search from then on

            if (pos + utf8len(terms[j]) > lineWidth){                        // end of word exceeds end of line
                int newCarriage = pos + (int)utf8len(terms[j]) - lineWidth;  // how many carracters should be printed
                                                                             //   in next line
                if (newCarriage > *carriage){
                    *carriage = newCarriage;
                }
            }
        }
    }
}







/**
 * Print a document along with its ID and score with the format specified.
 * This is the function client code will use.
 * @param text The text of the document for printing
 * @param docId  The ID of the document
 * @param score Its score of a /search command
 * @param terms All the words to be underlined
 * @param numTerms How many terms are there
 * @param resultIndex The ranking of that document in the query
 * @param K how many documents will be printed (for offset)
 * @param maxId what is the largest id for printing (for offset)
 */
void displayRelevantDocument(char *text, int docId, double score, char **terms, int numTerms, int resultIndex, int K, int maxId){
    char *formatedText = malloc((TABLNGTH*strlen(text)+1)*sizeof(char));    // get an overestimation of space
                                                            // for string a copy of the text with spaces instead of TABS
    int carriage = 0;
    int offset = getOffsetSize(score, maxId, K);                            // how many chars initial format will occupy
    char labeling[offset];                                                  // string for printing that format
    formatInitial(labeling, docId, resultIndex, score, maxId, K);           // create that format
    int terminalColumns = getTerminalColumns();
    int numLines = (int)utf8len(text) / (terminalColumns-offset);           // how many lines printing will need
                                                                            //           (without underlinings)
    if (utf8len(text) % (terminalColumns-offset) != 0) numLines++;          // also account for final line

    substituteTabs(text, formatedText, offset, terminalColumns);            // replace all TABS wih equivalent spaces
    formatedText = realloc(formatedText, (strlen(formatedText)+1)*sizeof(char)); // deallocate unused memory

    int lineNumber;
    for (lineNumber=0; lineNumber<numLines; lineNumber++){                  // for each line
        int i;
        if (lineNumber == 0){
            for (i=0; i<offset; i++) putchar(labeling[i]);                  // print labeling in first line
        }else{
            for (i=0; i<offset; i++) putchar(' ');                          // print spaces instead
        }

        int alreadyPrinted = lineNumber * (terminalColumns-offset);   // how many characters of the text we have printed

                                                                            // how many chars this line has
                                                                            //   (last line may have fewer)
        int lineWidth = (lineNumber != numLines-1) ? terminalColumns-offset : (int)strlen(formatedText)-alreadyPrinted;

        for (i=0; i<lineWidth; i++){                                        // print character
            putchar(formatedText [alreadyPrinted + i]);
        }
        putchar('\n'); fflush(stdout);


        char underliningString[lineWidth+1];                                // fix underlining string
        underline(underliningString, formatedText, alreadyPrinted, lineWidth, terms, numTerms, &carriage);

        if (strchr(underliningString, '^') != NULL) {                    // if there is a word to underline in that line
            for (i=0; i<offset; i++) putchar(' ');                          // print offset
            for (i=0; i<lineWidth; i++){
                putchar(underliningString[i]);                              // print the characters
            }
            putchar('\n'); fflush(stdout);
        }
    }
    free(formatedText);
}




/**
 * This prints document appearances as needed by the DF command.
 * To be used as a callback in Trie traversing.
 * @param word The word to be printed
 * @param postingList Its associated posting list
 */
void printDocumentAppearances(char *word, PostingList *postingList){
    if (!postingList){
        printf("%s 0\n",word);
        return;
    }
    int n = PostingList_getNumOfDocuments(postingList);
    printf("%s %d\n", word, n);
}