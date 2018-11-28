#include "../include/scoring.h"
#include <math.h> /* for log */


/**
 * Compute the inverse document frequency of a word
 * @param index An index to retrieve information
 * @param postingList The implied word's posting list
 * @return the IDF value of that word
 */
double IDF(Index *index, PostingList *postingList){
    double nominator = Index_size(index) - PostingList_getNumOfDocuments(postingList) + 0.5;
    double denominator = PostingList_getNumOfDocuments(postingList) + 0.5;
    return log(nominator / denominator);
}



/**
 * Find the socre of a document given the search terms of the query.
 * @param index The index to retrieve extra info
 * @param docId The document id for which the score will be computed
 * @param terms An array of search terms
 * @param numTerms How many terms are there
 * @return The socre of the document with the corresponding document id.
 */
double getScore(Index *index, int docId, char **terms, int numTerms) {
    const double k1 = 1.2;                                                // constants defined in the exercise
    const double b = 0.75;

    double score = 0;                                                     // start computing score here
    int i;
    for (i=0; i<numTerms; i++){                                           // for each term
        PostingList *pl = Index_getPostingList(index, terms[i]);
        if (!pl){                                                         // the word does not exist in the index
            continue;                                                     // no need to add anything
        }
        double idf = IDF(index, pl);                                      // get the IDF of that word
        int termFrequency = PostingList_getTermFrequency(pl, docId);      // the f(qi,D) value
        if (termFrequency == 0) continue;                                 // the word does not exist in that document

        double nominator = termFrequency * (k1+1);                        // start computing score
                                                                          // intermediate vars are easier for debugging
        int numWordsDocument = Index_getNumOfDocumentWords(index, docId); // the |D| value
        double avgdl = Index_getAverageDl(index);

        double denominator = termFrequency + k1 * ( 1- b + b * (numWordsDocument / avgdl));

        score += (idf * (nominator / denominator));
    }
    return score;
}

/**
 * A function for comparing two RelativeDocuments in terms of their score.
 * To be used for sorting.
 * @param lhs The one relative document
 * @param rhs The other relative document
 * @return 1 if lhs has a smaller score than rhs, 0 otherwise
 */
int compareScores(const void *lhs, const void *rhs){
    struct RelevantDocument *one = (struct RelevantDocument *)lhs;
    struct RelevantDocument *other = (struct RelevantDocument *)rhs;
    return one->score < other->score;
}

