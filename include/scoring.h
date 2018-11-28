#ifndef MINISEARCH_SCORING_H
#define MINISEARCH_SCORING_H

#include "postingList.h"
#include "index.h"

/**
 * A struct which includes each document's id and score.
 * It is needed for determining top K documents.
 * No need for abstraction here
 */
struct RelevantDocument{
    int docId;
    double score;
};


/**
 * Find the socre of a document given the search terms of the query.
 * @param index The index to retrieve extra info
 * @param docId The document id for which the score will be computed
 * @param terms An array of search terms
 * @param numTerms How many terms are there
 * @return The socre of the document with the corresponding document id.
 */
double getScore(Index *index, int docId, char **terms, int numTerms);

/**
 * A function for comparing two RelativeDocuments in terms of their score.
 * To be used for sorting.
 * @param lhs The one relative document
 * @param rhs The other relative document
 * @return 1 if lhs has a smaller score than rhs, 0 otherwise
 */
int compareScores(const void *lhs, const void *rhs);

#endif //MINISEARCH_SCORING_H
