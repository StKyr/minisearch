#include <stdlib.h> /* for malloc, free, NULL */
#include "../include/postingList.h"


typedef  struct pl_node Node; // forward definition of node structure needed due to recurse

/**
 * A posting list node which is a listnode containing the document id and the word's frequency
 */
struct pl_node {
    int docId;             /* the id of a document in which the implied word is found */
    int numOfAppearances;  /* how many times the word was found which is its frequency */
    Node *next;            /* pointing to the next document-node the word was found */
};

/**
 * The struct of the PostingList. It contains a pointer to the first document
 * and its size for retrieving the number of documents in which a word is found in constant time
 */
struct postingList_t{
    int size;   /* the number of nodes the list has */
    Node *head; /* the first document in which the implied word is found */
};


/**
 * Create an empty posting list.
 * @return A malloc'd pointer to an empty posting list.
 */
PostingList * PostingList_Create(){

    PostingList *list = malloc(sizeof(PostingList));
    list->head = NULL;
    list->size = 0;
}



/**
 * Traverse the list and notify node with specified docId that there is one more word appearance
 * ( by incrementing its frequency).
 * If there isn't such a node it will be created and set its frequency value to 1.
 * @param list A (pointer to a) posting list to be updated
 * @param docId The document id of a (perhaps non-existent) node.
 */
void PostingList_Update(PostingList *list, int docId){
    Node *current = list->head;

    while(current){
        if (current->docId == docId) break;
        current = current->next;
    }

    if (!current){
        current = malloc(sizeof(Node));
        current->docId = docId;
        current->numOfAppearances = 0;
        current->next = list->head;
        list->head = current;
        list->size++;
    }
    current->numOfAppearances++;
}


/**
 * Return how many documents this posting list have (which is equal to its number of nodes)
 * @param list A (pointer to a ) posting list to retrieve this information
 * @return The number of document this posting list contains.
 */
int PostingList_getNumOfDocuments(PostingList *list){
    return list->size;
}


/**
 * For a specific document id, return the word's frequency which is how many times the word appears in that document
 * @param list A (pointer to a ) posting list to be processed
 * @param docId The id of the desired document.
 * @return The frequency of the implied word in the desired document, or 0 if the docId does not exist.
 */
int PostingList_getTermFrequency(PostingList *list, int docId){
    Node *current = list->head;
    while(current){
        if (current->docId == docId) return current->numOfAppearances;
        current = current->next;
    }
    return 0;
}



/**
 * Retrieve the document ids in which the implied word is found.
 * @param list A (pointer to a ) posting list to be processed
 * @param documents The array in which the document ids will be stored. Must have adequate allocated space
 * (use `getNumOfDocuments()` for that)
 */

void PostingList_getDocuments(PostingList *list, int documents[]){

    int i=0;
    Node *current = list->head;
    while(current){
        documents[i++] = current->docId;
        current = current->next;
    }

}


/**
 * An auxiliary function for recursively freeing list nodes
 * @param node A node to be freed, while freeing firstly all its next ones
 */
void freeListRec(Node *node){
    if (node == NULL) return;
    freeListRec(node->next);
    free(node);

}


/**
 * Free up any allocated memory by also freeing up its nodes.
 * @param postingList A (pointer to a ) posting list to be freed.
 */
void PostingList_destroy(PostingList *postingList){
    if (!postingList) return;
    freeListRec(postingList->head);
    free(postingList);
}