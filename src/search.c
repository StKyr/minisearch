#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/search.h"
#include "../include/printer.h"
#include "../include/scoring.h"

/**
 * Find if an item exists in an array. A simple sequential search
 * @param array The array to be searched
 * @param size The number of elements it holds
 * @param item The item to search
 * @return `true` if item exists in array, `false` otherwise
 */
bool existsIn(const int array[], int size, int item){
    int i;
    for (i=0; i<size; i++) if (array[i] == item) return true;
    return false;
}


/**
 * Search the index to find all document ID's that have at least one term appearing in them.
 * @param index The index for searching
 * @param terms An array of search terms
 * @param numOfTerms How many terms are there
 * @param relativeIds An array of the resulting document ID's [output parameter]
 * @param numDocuments How many documents (document ID's) found [output parameter]
 */
void getRelevantIds(Index *index, char **terms, int numOfTerms, int **relativeIds, int *numDocuments){
    *numDocuments = 0;
                                                                            // count how many documents are there
                                                                            //    (as upper bound; we count a document
    int i;                                                                  //    that contains two words twice)
    for (i=0; i<numOfTerms; i++) {                                          // for each term
        PostingList *postingList = Index_getPostingList(index, terms[i]);   // get its posting list
        if (!postingList) continue;                                         // word does not exist in our index

        int numCurrDocuments = PostingList_getNumOfDocuments(postingList);  // number of documents this word exists in
        *numDocuments += numCurrDocuments;
    }

    *relativeIds = malloc(*numDocuments*sizeof(int));                       // allocate needed space

    int idsInserted = 0;
    for (i=0; i<=numOfTerms; i++){                                          // do the same process to insert the ID's
        PostingList *postingList = Index_getPostingList(index, terms[i]);
        if (!postingList) continue;

        int numCurrDocuments = PostingList_getNumOfDocuments(postingList);

        int termDocuments[numCurrDocuments];
        PostingList_getDocuments(postingList, termDocuments);               // retrieve all document ID's with this word

        int j;
        for (j=0; j<numCurrDocuments; j++){                                 // for each id
            if (existsIn(*relativeIds, idsInserted, termDocuments[j])){     // if we have inserted it before
                (*numDocuments)--;                                          // do not insert it twice
                continue;
            }
            (*relativeIds)[idsInserted] = termDocuments[j];                 // insert it
            idsInserted++;
        }
    }
    //*relativeIds = realloc(relativeIds, idsInserted*sizeof(int));           // deallocate unused space
}



/**
 * The business logic for /search command. It finds top K documents with highest score
 * given some search terms and prints them.
 * @param index An index for searching
 * @param terms An array of search terms
 * @param numOfTerms How many terms are there
 * @param K How many documents should be output
 */
void searchDocuments(Index *index, char **terms, int numOfTerms, int K){
    /**
     * Business logic:
     * 1. Get all document ID's that contain at least one term [named "relevant documents"]
     * 2. Insert first K of them in an array
     * 3. Sort them by score
     * 4. For any other relevant document
     * i.   compute its score
     * ii.  find its correct position in the array (if any) in terms of score
     * iii. place it there and shift the rest of them (the ones with lower score) downwards (discarding the last one)
     * 5. print the final instance of the array
     */
    int *relevantIds;
    int numDocuments;
    getRelevantIds(index, terms, numOfTerms, &relevantIds, &numDocuments);    // 1. retrieve al relevant document IDs

    int minNumDocs = (numDocuments < K) ? numDocuments : K;                   // size of the array
                                                                              // relevant documents may be fewer than K
    struct RelevantDocument relevantDocuments[minNumDocs];           // 2. initialize the array with the first documents
    int i;
    for (i=0; i<minNumDocs; i++){
        relevantDocuments[i].docId = relevantIds[i];
        relevantDocuments[i].score = getScore(index, relevantDocuments[i].docId, terms, numOfTerms);
    }
    qsort(relevantDocuments, (size_t )minNumDocs, sizeof(struct RelevantDocument), compareScores); // 3. sort them

   for (i=minNumDocs; i<numDocuments; i++){                                   // for any other document
       struct RelevantDocument document;
       document.docId = relevantIds[i];
       document.score = getScore(index, document.docId, terms, numOfTerms);   // 4i. compute its score

       int j,k;
       bool placeFound = false;
       for (j=0; j<minNumDocs; j++){                                          // 4ii. locate its position in the array
           if (document.score >=  relevantDocuments[j].score){
               placeFound = true;
               break;
           }
       }
       if (placeFound){                                                       // 4iii. if should be in the array
           for (k=minNumDocs-1; k>j; k--){                                    // shift all "lower" documents downwards
               relevantDocuments[k] = relevantDocuments[k-1];
           }
           relevantDocuments[j] = document;                                   // place that document in the array, too
       }
    }

    for (i=0; i<minNumDocs; i++){                                             // 5. print results
        displayRelevantDocument(Index_getDocument(index, relevantDocuments[i].docId),
                                relevantDocuments[i].docId,
                                relevantDocuments[i].score,
                                terms,
                                numOfTerms,
                                i,
                                K,
                                Index_size(index));
    }
    free(relevantIds);
}



/**
 * Business logic for /tf command.
 * @param index An index for searching
 * @param id The id of some document
 * @param word A term to get its frequency
 */
void searchTermFrequency(Index *index, int id, char *word){

    if (id >= Index_size(index)){                         // no document of such ID
        printf("%d 0 %s\n", id, word);
        return;
    }

    PostingList *pl = Index_getPostingList(index, word);
    if (! pl){                                            // no such word in the index
        printf("%d 0 %s\n", id, word);
        return;
    }

    int frequency = PostingList_getTermFrequency(pl,id);  // retrieve frequency of term
    printf("%d %d %s\n", id, frequency, word);
}





/**
 * The business logic function for /df command
 * @param index An index for searching
 * @param word A term to search its document frequency or NULL for printing all document frequencies
 */
void searchDocumentFrequency(Index *index, char *word){
    if (!word){                                                      // no term specified
        Index_accessAllWords(index, printDocumentAppearances);       // need to do a full Trie traversal
                                                                     // we will use `printDocumentAppearances` as
                                                                     //   a callback for printing each found word
    }else{                                                           // we have a term
        PostingList *pl = Index_getPostingList(index, word);
        printDocumentAppearances(word, pl);
    }
}




























