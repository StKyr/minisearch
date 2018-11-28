#ifndef MINISEARCH_UITL_H
#define MINISEARCH_UITL_H

#include "postingList.h"


/**
 * Print a document along with its ID and score with the format specified.
 * @param text The text of the document for printing
 * @param docId  The ID of the document
 * @param score Its score of a /search command
 * @param terms All the words to be underlined
 * @param numTerms How many terms are there
 * @param resultIndex The ranking of that document in the query
 * @param K how many documents will be printed (for offset)
 * @param maxId what is the largest id for printing (for offset)
 */
void displayRelevantDocument(char *text, int docId, double score, char **terms, int numTerms, int resultIndex,
                             int K, int maxId);

/**
 * This prints document appearances as needed by the DF command.
 * To be used as a callback in Trie traversing.
 * @param word The word to be printed
 * @param postingList Its associated posting list
 */
void printDocumentAppearances(char *word, PostingList *postingList);

#endif //MINISEARCH_UITL_H
