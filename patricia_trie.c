#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "patricia_trie.h"

PATRICIA_TRIE *patricia_trie_create() {
	PATRICIA_TRIE *trie = (PATRICIA_TRIE *)malloc(sizeof(PATRICIA_TRIE));
	
	trie->nchilds = 0;
	trie->nallocchilds = 256;	// Possible charaters
	trie->childs = (PATRICIA_TRIE **)calloc(trie->nallocchilds, sizeof(PATRICIA_TRIE *));	// Childs with NULLs
	
	trie->key = "";
	trie->data = NULL;
	
	return trie;
}

int patricia_trie_add(PATRICIA_TRIE *patricia_trie, char *key, void *element) {
	int i, j;
	
// 	printf("key:%s patritia_key:%s\n",key, patricia_trie->key);
	// Compares the string
	for(i=0; patricia_trie->key[i]==key[i] && key[i]!='\0' && patricia_trie->key[i]!='\0'; i++);
	char *ch_k = &(key[i]);
	char *ch_p = &(patricia_trie->key[i]);
// 	printf("%i - ch_k:[%d]%s ch_p:[%d]%s\n",i, *ch_k, ch_k, *ch_p, ch_p);
	
	///////////////////////////
	// Key is equal to the trie
	if(*ch_k=='\0' && *ch_p=='\0') {
// 		puts("Key is equal to the trie");
		patricia_trie->data = element;
		return 1;
	}
	
	////////////
	// Next node
	if(*ch_p == '\0' && ch_k != '\0') {
// 		puts("Next node");
		PATRICIA_TRIE *next_node  = patricia_trie->childs[ (int)*ch_k ];
			
		if(next_node == NULL) {
			next_node = patricia_trie_create();
			next_node->key = (char *)malloc(200*sizeof(char));
			strcpy(next_node->key, ch_k);
			
			patricia_trie->childs[ (int)*ch_k ] = next_node;
			patricia_trie->nchilds++;
		}
		
		return patricia_trie_add(next_node, ch_k, element);
	}
	
	
	////////////////
	// Breaks a node
	if(*ch_k != *ch_p && *ch_p != '\0') {
// 		puts("Breaks a node");
		// Creates a node to keep the old subtrie
		PATRICIA_TRIE *keep_node = patricia_trie_create();
		keep_node->key = (char *)malloc(200*sizeof(char));
		strcpy(keep_node->key, ch_p);	// Copies the remaining key
		
		keep_node->nallocchilds = patricia_trie->nallocchilds;
		keep_node->nchilds = patricia_trie->nchilds;
		for(j=0; j< patricia_trie->nallocchilds; j++)		// Childs
			keep_node->childs[j] = patricia_trie->childs[j];
		
		keep_node->data = patricia_trie->data;	// Data
		
		
		// Updates with the new information the trie
		for(j=0; j<patricia_trie->nallocchilds; j++)
			patricia_trie->childs[j] = NULL;
		patricia_trie->nchilds = 2;
		patricia_trie->key[i] = '\0';
		patricia_trie->data = NULL;
		
		// Adds the node to keep the old subtrie
		patricia_trie->childs[ (int)keep_node->key[0] ] = keep_node;
		
		// If the broken node creates the desired key
		if(*ch_k == '\0') {
			patricia_trie->data = element;
			return 1;
		} // Otherwise...
		
		// Creates the new node
		PATRICIA_TRIE *new_node = patricia_trie_create();
		new_node->key = (char *)malloc(200*sizeof(char));
		strcpy(new_node->key, ch_k);	// Copies the new key
		
		patricia_trie->childs[ (int)new_node->key[0] ] = new_node;
		
		return patricia_trie_add(new_node, ch_k, element);
	}
	
	fputs("Error on patricia_trie_add", stderr);
	return 0;	// ERROR
}

int patricia_trie_remove(PATRICIA_TRIE *patricia_trie, char *key);
void *patricia_trie_search(PATRICIA_TRIE *patricia_trie, char *key);
void patricia_trie_delete(PATRICIA_TRIE *patricia_trie);


void patricia_trie_print(PATRICIA_TRIE *patricia_trie, int level) {
	int i;
	
	if(patricia_trie == NULL)
		return;
	
	//for(i=0; i<level; i++)
	//	printf("    ");
	//printf("%s (%s)\n", patricia_trie->key, patricia_trie->data);
	if(patricia_trie->data != NULL)
		printf("%s\n", (char *)patricia_trie->data);
	
	for(i=0; i<patricia_trie->nallocchilds; i++)
		patricia_trie_print(patricia_trie->childs[i], level+1);
}

void patricia_trie_print_node(PATRICIA_TRIE* patricia_trie) {
	int i;
	
	puts  ("NODE: ");
	printf("      Key: %s\n", patricia_trie->key);
	printf("  NChilds: %d\n", patricia_trie->nchilds);
	printf("     Data: %s\n", (char *)patricia_trie->data);
	puts  ("   Childs:");
	for(i=0; i<patricia_trie->nallocchilds; i++)
		if(patricia_trie->childs[i] != NULL)
			printf("[%d]%c: %s\n", i, i, patricia_trie->childs[i]->key);
}
