#ifndef _PATRICIA_TRIE_INC
#define _PATRICIA_TRIE_INC


typedef struct PATRICIA_TRIE_STRUCT PATRICIA_TRIE;

struct PATRICIA_TRIE_STRUCT {
	char *key;
	
	PATRICIA_TRIE **childs;
	int nchilds;
	int nallocchilds;
	
	void *data;
};


PATRICIA_TRIE *patricia_trie_create();
int patricia_trie_add(PATRICIA_TRIE *patricia_trie, const char *key, void *element);
void *patricia_trie_search(PATRICIA_TRIE *patricia_trie, const char *key);
void patricia_trie_iterate(PATRICIA_TRIE *patricia_trie, void *info, int (*action)(void *info, void *data));
int patricia_trie_remove(PATRICIA_TRIE *patricia_trie, const char *key);
void patricia_trie_delete(PATRICIA_TRIE *patricia_trie);
void patricia_trie_print(PATRICIA_TRIE *patricia_trie, int level);
void patricia_trie_print_node(PATRICIA_TRIE* patricia_trie);

#endif	/* _PATRICIA_TRIE_INC */
