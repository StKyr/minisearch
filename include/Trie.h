#ifndef MINISEARCH_TRIE_H
#define MINISEARCH_TRIE_H


#include "postingList.h"

/**
 * The Trie data type
 */
typedef struct trie_t Trie;

/**
 * Initialize a new Trie object.
 * @param trie A double pointer to a trie for allocating space and initializing its content.
 */
void Trie_init(Trie **trie);

/**
 * Insert a word to a Trie, either updating its posting list (by 1) or creating a new one.
 * @param trie A trie for inserting
 * @param word The word to be inserted
 * @param docId The document ID in which the word was found
 */
void Trie_insert(Trie *trie, char *word, int docId);

/**
 * Search the Trie tree for getting the posting list of a word
 * @param trie A Trie for searching
 * @param word The word to search
 * @return A pointer to this word's posting list or NULL if the word does not exist in the Trie
 */
PostingList *Trie_retrievePostingList(Trie *trie, char *word);

/**
 * Full Trie traversal. Each time a terminal node (end of word) is visited, a callback is invoked.
 * @param trie The trie to traverse
 * @param callback A function taking as an argument a word from the trie and its posting list.
 * It is meant to be used for printing purposes.
 */
void Trie_accessAllWords(Trie *trie, void (*callback)(char *word, PostingList *pl));

/**
 * Free up all allocated space of the object. Will also free attached posting lists.
 * @param trie A trie to be destroyed
 */
void Trie_destroy(Trie *trie);
#endif //MINISEARCH_TRIE_H
