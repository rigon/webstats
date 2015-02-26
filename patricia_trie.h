

#ifndef _PATRICIA_TRIE_INC
#define _PATRICIA_TRIE_INC


typedef struct {
	
	
} PATRICIA_TRIE;


PATRICIA_TRIE *patricia_trie_create();
int patricia_trie_add(PATRICIA_TRIE *patricia_trie, void *element);
int patricia_trie_remove(PATRICIA_TRIE *patricia_trie, void *element);
void *patricia_trie_search(PATRICIA_TRIE *patricia_trie, const void *element);

#endif	/* _PATRICIA_TRIE_INC */
