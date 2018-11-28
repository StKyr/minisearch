#include <stdio.h>            /* for FILE, fopen, fclose, fgetc, EOF, getline */
#include <stdlib.h>           /* for malloc, free, strtoll */
#include <memory.h>           /* for strtok, strlen, strcpy */

#include "../include/index.h"
#include "../include/Trie.h"


/**
 * A struct containing the text of a document along with the number of its words
 * for constant time retrieval. This is the actual data Map stores.
 */
struct Document{
    int numOfWords;
    char *text;
};


/**
 * The MAP structure. It stores documents in an array,
 * plus it contains information abou the total number of documents along with avgdl for housekeeping.
 */
typedef struct mapindex {
    int size;
    double avgdl;
    struct Document *documents;

} Map;


/**
 * The Index struct which only encapsulates the Trie and the Map
 * for providing an easier interface to the outer world.
 */
struct index_t{
    Map map;
    Trie *invertedIndex; //pointer needed because of abstraction
};



/**
 * A preparing function which only traverses the input file to get how many lines there are.
 * @param docfile The file containing the documents
 * @return The number of lines this file has.
 */
int getNumberOfDocuments(char *docfile){
    FILE *fin = fopen(docfile, "r");
    if (!fin) return 0;

    int n=1;
    int ch;
    while((ch = fgetc(fin)) != EOF) if (ch == '\n') n++;
    fclose(fin);

    return n;
}


/**
 * Parse input file and build Map and Inverted Index structs.
 * @param pIndex A double pointer to an index struct for initalizing.
 * @param docfile A string containing the name (and path) of the input file.
 * @return 0 on successful parsing, non-zero error code otherwise.
 */
int buildIndex(Index **index, char *docfile){

    *index = malloc(sizeof(Index));                                // initialize members
    Trie_init(&((*index)->invertedIndex));
    (*index)->map.size = 0;
    (*index)->map.avgdl = 0.0;
    (*index)->map.documents = malloc(getNumberOfDocuments(docfile)*sizeof(struct Document));

    FILE *fin = fopen(docfile, "r");                               // open input file
    if (fin == NULL) return -1;                                    // bad file: non existent, no permissions or other

    char *line = NULL;                                             // prepare line buffer
    size_t lineSize = 0;
    int getline_code = (int) getline(&line, &lineSize, fin);       // read first line. It will also allocate memory


    int numDocuments = 0;
    while (line && getline_code != -1){                            // parse line after line

        int length = getline_code;
        if (line[length-1] == '\n') line[length-1] ='\0';          // manually remove trailing \n character

        char *idstr = strtok(line, " \t");                         // split line to ID part and TEXT
        if (idstr == NULL){                                        // an empty line
            fclose(fin);
            free(line);
            return -3;
        }

        size_t textOffset = idstr-line;
        textOffset += strlen(idstr)+1;
        char *text = line+textOffset;                              // the rest of the line i.e. the document text

        int id = (int)strtol(idstr,NULL,10);                       // parse ID
        if (id != numDocuments) {                                  // not a number or an invalid number
            fclose(fin);
            free(line);
            return -2;
        }

                                                                   // insert new document in MAP
        (*index)->map.documents[id].text = malloc((strlen(text)+1)*sizeof(char));
        strcpy((*index)->map.documents[id].text, text);
        (*index)->map.documents[id].numOfWords = 0;

        char *word;
        while ((word= strtok(NULL, " \t")) != NULL){               // loop through all words of document
            Trie_insert((*index)->invertedIndex, word, id);        // insert each one into TRIE
            (*index)->map.documents[id].numOfWords++;              // update MAP's member tracking this document's words
//fprintf(stderr, "BuildIndex: inserted \"%s\"\n", word);
        }

                                                                   // update INDEX's (Map's) members
        (*index)->map.avgdl += (*index)->map.documents[id].numOfWords;
        numDocuments++;

        getline_code = (int) getline(&line, &lineSize, fin);
    }

    (*index)->map.size = numDocuments;                             // finalize Map's members
    (*index)->map.avgdl /= numDocuments;

    free(line);
    fclose(fin);
    printf(">");
    return 0;
}







/**
 * Free the index and the underlying structures.
 * @param pIndex A double pointer to the index to be destroyed.
 */
void Index_Destroy(Index **pIndex){
    Trie_destroy((*pIndex)->invertedIndex);
    free((*pIndex)->invertedIndex);

    int i;
    for (i=0; i<(*pIndex)->map.size ; i++){
        free((*pIndex)->map.documents[i].text);
    }
    free((*pIndex)->map.documents);

    free(*pIndex);
    *pIndex = NULL;
}




/**
 * Retrieve a posting list of a word from the inverted index
 * @param index A (pointer to an) index struct containing the inverted index.
 * @param word The word for which the posting list will be retrieved.
 * @return A pointer to the wanted posting list or NULL if no such word is stored.
 */
PostingList *Index_getPostingList(Index *index, char *word){
    return Trie_retrievePostingList(index->invertedIndex, word);
}


/**
 * A generic traverse of the whole inverted index so as to finally access al words / posting lists stored.
 * Each time a posting list is reached the given callback is invoked in which the appropriate word is also passed.
 * It is meant to be used when /df is typed without any parameters.
 * @param index A (pointer to an) index to traverse and access all its words.
 * @param callback A function to perform the desired action when a posting list is accessed.
 *  The word associated with the posting list is also passed in the callback.
 */
void Index_accessAllWords(Index *index, void (*callback)(char *word, PostingList *pl)){
    Trie_accessAllWords(index->invertedIndex, callback);
}


/**
 * Return the number of documents saved in the Map.
 * @param index A (pointer to an) index containing the map structure.
 * @return The total number of documents the map contains
 */
int Index_size(Index *index){
    return index->map.size;
}


/**
 * For a specific document id, retrieve how many words this document contains.
 * This is a constant time operation.
 * @param index A (pointer to an) index in which the document is stored.
 * @param docId The ID of the desired document
 * @return The number of words the document with `docId` has or 0 if no such document is stored.
 */
int Index_getNumOfDocumentWords(Index *index, int docId){
    if (docId >= index->map.size) return 0;
    return index->map.documents[docId].numOfWords;
}


/**
 * Retrieve the average document length of the documents stored in the index.
 * This is a constant time operation.
 * @param index A (pointer to an) index to be processed.
 * @return The "avgdl" value.
 */
double Index_getAverageDl(Index *index){
    return index->map.avgdl;
}


/**
 * Retrieve the text of a document
 * @param index A (pointer to an) index to be processed.
 * @param docId The document id of the desired document.
 * @return A pointer to the beginning of the text of the document (a C string) or NULL if no such document exists.
 */
char *Index_getDocument(Index *index, int docId){
    if (docId >= index->map.size) return NULL;
    return index->map.documents[docId].text;
}