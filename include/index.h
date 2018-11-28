#ifndef MINISEARCH_INDEX_H
#define MINISEARCH_INDEX_H

#include "postingList.h"

/**
 * The top level abstraction of the program. It is merely an encapsulation structure for
 * Map and Inverted Index (Trie), containing wrapper functions
 * while it also responsible for parsing the input file to build the underlying structures
 * and for deallocating them.
 * It is the only struct meant to be used by main client code.
 */
typedef struct index_t Index;

/**
 * Parse input file and build Map and Inverted Index structs.
 * @param pIndex A double pointer to an index struct for initalizing.
 * @param docfile A string containing the name (and path) of the input file.
 * @return 0 on successful parsing, non-zero error code otherwise.
 */
int buildIndex(Index **pIndex, char *docfile);

/**
 * Retrieve a posting list of a word from the inverted index
 * @param index A (pointer to an) index struct containing the inverted index.
 * @param word The word for which the posting list will be retrieved.
 * @return A pointer to the wanted posting list or NULL if no such word is stored.
 */
PostingList *Index_getPostingList(Index *index, char *word);

/**
 * A generic traverse of the whole inverted index so as to finally access al words / posting lists stored.
 * Each time a posting list is reached the given callback is invoked in which the appropriate word is also passed.
 * It is meant to be used when /df is typed without any parameters.
 * @param index A (pointer to an) index to traverse and access all its words.
 * @param callback A function to perform the desired action when a posting list is accessed.
 *  The word associated with the posting list is also passed in the callback.
 */
void Index_accessAllWords(Index *index, void (*callback)(char *word, PostingList *pl));

/**
 * Return the number of documents saved in the Map.
 * @param index A (pointer to an) index containing the map structure.
 * @return The total number of documents the map contains
 */
int Index_size(Index *index);

/**
 * For a specific document id, retrieve how many words this document contains.
 * This is a constant time operation.
 * @param index A (pointer to an) index in which the document is stored.
 * @param docId The ID of the desired document
 * @return The number of words the document with `docId` has or 0 if no such document is stored.
 */
int Index_getNumOfDocumentWords(Index *index, int docId);

/**
 * Retrieve the average document length of the documents stored in the index.
 * This is a constant time operation.
 * @param index A (pointer to an) index to be processed.
 * @return The "avgdl" value.
 */
double Index_getAverageDl(Index *index);

/**
 * Retrieve the text of a document
 * @param index A (pointer to an) index to be processed.
 * @param docId The document id of the desired document.
 * @return A pointer to the beginning of the text of the document (a C string) or NULL if no such document exists.
 */
char *Index_getDocument(Index *index, int docId);

/**
 * Free the index and the underlying structures.
 * @param pIndex A double pointer to the index to be destroyed.
 */
void Index_Destroy(Index **pIndex);

#endif //MINISEARCH_INDEX_H
