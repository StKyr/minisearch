#ifndef MINISEARCH_POSTINGLIST_H
#define MINISEARCH_POSTINGLIST_H

/**
 * A posting list type as defined in the exercise
 */
typedef struct postingList_t PostingList;

/**
 * Create an empty posting list.
 * @return A malloc'd pointer to an empty posting list.
 */
PostingList * PostingList_Create();

/**
 * Traverse the list and notify node with specified docId that there is one more word appearance
 * ( by incrementing its frequency).
 * If there isn't such a node it will be created and set its frequency value to 1.
 * @param list A (pointer to a) posting list to be updated
 * @param docId The document id of a (perhaps non-existent) node.
 */
void PostingList_Update(PostingList *list, int docId);

/**
 * Return how many documents this posting list have (which is equal to its number of nodes)
 * @param list A (pointer to a ) posting list to retrieve this information
 * @return The number of document this posting list contains.
 */
int PostingList_getNumOfDocuments(PostingList *list);

/**
 * For a specific document id, return the word's frequency which is how many times the word appears in that document
 * @param list A (pointer to a ) posting list to be processed
 * @param docId The id of the desired document.
 * @return The frequency of the implied word in the desired document, or 0 if the docId does not exist.
 */
int PostingList_getTermFrequency(PostingList *list, int docId);

/**
 * Retrieve the document ids in which the implied word is found.
 * @param list A (pointer to a ) posting list to be processed
 * @param documents The array in which the document ids will be stored. Must have adequate allocated space
 * (use `getNumOfDocuments()` for that)
 */
void PostingList_getDocuments(PostingList *list, int documents[]);

/**
 * Free up any allocated memory by also freeing up its nodes.
 * @param postingList A (pointer to a ) posting list to be freed.
 */
void PostingList_destroy(PostingList *postingList);



#endif //MINISEARCH_POSTINGLIST_H
