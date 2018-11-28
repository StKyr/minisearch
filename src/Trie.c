#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "../include/Trie.h"
#include "../include/postingList.h"


typedef struct  trieNode_t TrieNode;


/**
 * A structure of a node of a Trie.
 */
struct trieNode_t{
    char letter;                // the letter of the node
    TrieNode *sibling;          // same level nodes (they share the same father)
    TrieNode *child;            // node with the next letter of the word
    PostingList *postingList;   // a posting list (for terminal nodes only)
};

/**
 * Initialize a new node
 * @param lttr the letter of the noda
 * @return An address to an initialized node (with letter inserted). Child, sibling and postring list are NULL.
 */
TrieNode *TrieNode_Create(char lttr){
    TrieNode *node = malloc(sizeof(TrieNode));
    node->letter = lttr;
    node->sibling = NULL;
    node->child = NULL;
    node->postingList = NULL;
    return node;
}


/**
 * Link sibling to the right of node. Will also rellocate all of node's siblings to the right of sibling
 * @param node
 * @param sibling
 */
void TrieNode_addSibling(TrieNode *node, TrieNode *sibling){
    TrieNode *prevSibling = node->sibling;
    node->sibling = sibling;
    sibling->sibling = prevSibling;
}

/**
 * Link a node with its child
 * @param node
 * @param child
 */
void TrieNode_addChild(TrieNode *node, TrieNode *child){
    node->child = child;
}

/**
 * Recursively insert a word and either update its posting list or it create anew.
 * @param node The current node of the recursion
 * @param remainingWord A pointer to the remaining part of the word
 * @param docId The id of the document for the posting list
 */
void TrieNode_Insert(TrieNode *node, char *remainingWord, int docId){
    char currLetter = remainingWord[0];                                      // the letter we are dealing with

    if (currLetter ==  node->letter){                                        // we are at a correct node

        if (strlen(remainingWord)>1){                                        // the word is not finished

            if (node->child != NULL){                                        // there is a node beneath
                TrieNode_Insert(node->child, remainingWord+1, docId);        // recurse there
            } else {                                                         // there is no node beneath
                TrieNode *childNode = TrieNode_Create(remainingWord[1]);     // create it anew
                TrieNode_addChild(node, childNode);                          // link it
                TrieNode_Insert(childNode, remainingWord+1, docId);          // recurse there
            }

        }else{                                                               // reached the end of the word
            if (!node->postingList) node->postingList = PostingList_Create();// the word was not previously inserted
            PostingList_Update(node->postingList, docId);                    // update the list
        }

    }else{                                                                   // the letter is not the same
                                                                             // need to search for the correct sibling
        if (currLetter < node->letter) {                                     // sibling ought to be on our left
                                                                             // we will create a new on our right
                                                                             // we will copy all our data there
                                                                             // and then we will override our data
                                                                             // with the new information
            TrieNode *copyNode = TrieNode_Create(node->letter);
            TrieNode_addChild(copyNode, node->child);
            copyNode->postingList = node->postingList;

            node->letter = currLetter;
            node->postingList = NULL;
            node->child = NULL;
            TrieNode_addSibling(node, copyNode);
            TrieNode_Insert(node, remainingWord, docId);                     // continue recursion form our node
                                                                             // which now has new info
        }else{                                                               // node should be on our right
            while (node->sibling != NULL){                                   // search for appropriate sibling

                if (node->sibling->letter == currLetter){                    // that sibling found
                    TrieNode_Insert(node->sibling, remainingWord, docId);    // recurse here
                    break;

                }else if (node->sibling->letter > currLetter){               // letter should be placed in between
                    TrieNode *newNode = TrieNode_Create(currLetter);         // create a node for that
                    TrieNode_addSibling(node, newNode);                      // link it properly
                    TrieNode_Insert(newNode, remainingWord, docId);          // recurse
                    break;
                }
                node = node->sibling;
            }

            if (node->sibling == NULL){                                      // searched all, letter should be the right
                TrieNode *lastNode = TrieNode_Create(currLetter);
                TrieNode_addSibling(node, lastNode);
                TrieNode_Insert(lastNode, remainingWord, docId);
            }
        }
    }
}


/**
 * Recursively descend until the end of the word is found. Return that posting list
 * @param node The current node of the recursion.
 * @param word A pointer to the remaining part of the word we are searching for
 * @return The posting list associated with the original word.
 */
PostingList* TrieNode_Descend(TrieNode *node, char * word){
    if (!node) return NULL;                                 // no word found

    if (node->letter == word[0]){                           // first letter of the (remaining) word is same as node's
        if (strlen(word) == 1){                             // we are at the end of the word
            return node->postingList;
        }else{
            return TrieNode_Descend(node->child, word+1);  // we need to go deeper (incrementing word's pointer)
        }

    }else{                                                 // node's letter is not the same as (remaining) word's letter
        TrieNode *appropriateSibling = node->sibling;      // we need to find the appropriate sibling then

        while(appropriateSibling){
            if (appropriateSibling->letter == word[0]) break;     // sibling found
            appropriateSibling = appropriateSibling->sibling;
        }

        if (appropriateSibling){
            return TrieNode_Descend(appropriateSibling, word);   // continue our quest there
        }else{                                                   // no sibling found
            return NULL;                                         // word's not in index
        }
    }
}


/**
 * A full Trie traversal. Every time a terminal node is reached, a callback is invoked.
 * @param node The current node of the recursive traversal process
 * @param word A stack of letters we have encountered so far
 * @param i Current depth (index of last character in word)
 * @param action A callback taking a word and a posting list
 */
void TrieNode_Traverse(TrieNode *node, char *word, int i, void (*action)(char *word, PostingList *pl)){
    word[i] = node->letter;                                   // place in stack the node letter

    if (node->postingList){                                   // reached a terminal node
        word[i+1] = '\0';                                     // "transform" the stack to a string
        action(word, node->postingList);                      // do whatever you have to do
    }
    if (node->child){                                         // Next thing is to go to our child
        TrieNode_Traverse(node->child, word, i+1, action);
    }

    if (node->sibling){                                       // Now do the same to the same level nodes
        TrieNode_Traverse(node->sibling, word, i, action);
        word[i] = node->letter;                               // re insert the node letter in the stack
    }
}






/**
 * Free up recursively a Trie node
 * @param node a node to be freed
 */
void TrieNode_Delete(TrieNode *node){
    if (node->postingList){                      // a terminal node
        PostingList_destroy(node->postingList);  // should free its posting list
    }
    if (node->sibling){                          // it has same leve nodes
        TrieNode_Delete(node->sibling);          // free them first
    }
    if (node->child){                            // it also has a child
        TrieNode_Delete(node->child);            // go there first
    }
    free(node);                                  // everything is clean, destroy myself.
}






/* ----------- TRIE_* function section ------------------------------------------*/



/**
 * The data structure of the Trie. It just contains a pointer to a root
 * and its maximum depth (as height). The latter will make traversing it easier and faster.
 */
struct trie_t{
    TrieNode *root;
    int height;
};


/**
 * Initialize a new Trie object.
 * @param trie A double pointer to a trie for allocating space and initializing its content.
 */
void Trie_init(Trie **trie){
    *trie = malloc(sizeof(Trie));
    (*trie)->root = TrieNode_Create('\0');    // dummy value. Doesn't matter anyway
    (*trie)->height = 0;
}




/**
 * Insert a word to a Trie, either updating its posting list (by 1) or creating a new one.
 * @param trie A trie for inserting
 * @param word The word to be inserted
 * @param docId The document ID in which the word was found
 */
void Trie_insert(Trie *trie, char *word, int docId){
    if (!word || strlen(word)==0) return;              // just to be safe from bad calls

    if (strlen(word)+1 > trie->height){                // update my `height` member (makes traversing easier/faster)
        trie->height = (int)strlen(word)+1;
    }
    if (trie->root->child == NULL){                    // struct is empty
        trie->root->child = TrieNode_Create(word[0]);  // create manually the first child
    }
    TrieNode_Insert(trie->root->child, word, docId);   // start recursive descending
}




/**
 * Search the Trie tree for getting the posting list of a word
 * @param trie A Trie for searching
 * @param word The word to search
 * @return A pointer to this word's posting list or NULL if the word does not exist in the Trie
 */
PostingList *Trie_retrievePostingList(Trie *trie, char *word){
    if (!word || strlen(word)==0) return NULL;          // no word given. just be safe
    return TrieNode_Descend(trie->root->child, word);   // descend tree from root
}


/**
 * Full Trie traversal. Each time a terminal node (end of word) is visited, a callback is invoked.
 * @param trie The trie to traverse
 * @param callback A function taking as an argument a word from the trie and its posting list.
 * It is meant to be used for printing purposes.
 */
void Trie_accessAllWords(Trie *trie, void (*callback)(char *word, PostingList *pl)){
    char word[trie->height];
    if (! trie->root->child) return;                             // empty data structure. Job done.
    TrieNode_Traverse(trie->root->child, word, 0, callback);     // start recursions from root
}



/**
 * Free up all allocated space of the object. Will also free attached posting lists.
 * @param trie A trie to be destroyed
 */
void Trie_destroy(Trie *trie){
    TrieNode_Delete(trie->root); // just delete the root
}
