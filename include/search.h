#ifndef MINISEARCH_SEARCH_H
#define MINISEARCH_SEARCH_H


#include "index.h"


/**
 * The business logic for /search command. It finds top K documents with highest score
 * given some search terms and prints them.
 * @param index An index for searching
 * @param terms An array of search terms
 * @param numOfTerms How many terms are there
 * @param K How many documents should be output
 */
void searchDocuments(Index *index, char **terms, int numOfTerms, int K);

/**
 * The business logic function for /df command
 * @param index An index for searching
 * @param word A term to search its document frequency or NULL for printing all document frequencies
 */
void searchDocumentFrequency(Index *index, char *word);


/**
 * Business logic for /tf command.
 * @param index An index for searching
 * @param id The id of some document
 * @param word A term to get its frequency
 */
void searchTermFrequency(Index *index, int id, char *word);


#endif //MINISEARCH_SEARCH_H
