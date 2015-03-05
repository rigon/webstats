/*
 * LogFormat "%v:%p %h %l %u %t \"%r\" %>s %O \"%{Referer}i\" \"%{User-Agent}i\"" vhost_combined
 * LogFormat "%h %l %u %t \"%r\" %>s %O \"%{Referer}i\" \"%{User-Agent}i\"" combined
 * LogFormat "%h %l %u %t \"%r\" %>s %O" common
 * LogFormat "%{Referer}i -> %U" referer
 * LogFormat "%{User-agent}i" agent
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logparser.h"
#include "patricia_trie.h"


int main(int argc, char **argv) {
	PATRICIA_TRIE *p = patricia_trie_create();
	/*puts("-- 1");
	printf("%d\n", patricia_trie_add(p, "MARIA", "MARIA"));
	puts("-- 2");
	printf("%d\n", patricia_trie_add(p, "MADALENA", "MADALENA"));
	puts("-- 3");
	printf("%d\n", patricia_trie_add(p, "MARIZA", "MARIZA"));
	puts("-- 4");
	printf("%d\n", patricia_trie_add(p, "MARIANA", "MARIANA"));
	printf("%d\n", patricia_trie_add(p, "MANUEL", "MANUEL"));
	printf("%d\n", patricia_trie_add(p, "MANUELA", "MANUELA"));
	printf("%d\n", patricia_trie_add(p, "MIGUEL", "MIGUEL"));
	printf("%d\n", patricia_trie_add(p, "MARCOS", "MARCOS"));
	printf("%d\n", patricia_trie_add(p, "MARCO", "MARCO"));
	printf("%d\n", patricia_trie_add(p, "MARCO", "MARCO"));
	printf("%d\n", patricia_trie_add(p, "MONICA", "MONICA"));
	printf("%d\n", patricia_trie_add(p, "DANIELA", "DANIELA"));
	puts("-- PRINT");
	patricia_trie_print(p, 0);
	
	patricia_trie_print_node(p->childs['M']);
	
	return EXIT_SUCCESS; */
	////////////////////////////////////
	
	// Arguments
	if(argc != 2) {
		fprintf(stderr, "Wrong number of arguments!\n");
		fprintf(stderr, "Usage: %s apache_log_file\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	// Open log
	FILE *log = fopen(argv[1], "rt");
	if(log == NULL) {
		fprintf(stderr, "Cannot open log file '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}
	
	// Reads the log file
	char buf[LINE_WIDTH];
	int i=0;
	while( fgets(buf, LINE_WIDTH, log) != NULL ) {
		//fflush(stdout);
		//printf("PRINT[%d] %s\n", ++i, buf);
		//fflush(stdout);
		//printf("%s",buf);
		
		REQUEST *req = parse_line(buf);
		//fflush(stdout);
		//printf("END REQUEST\n");
		//fflush(stdout);
		patricia_trie_add(p, req->ip, req->ip);
	
		
	}
	puts("CLOSE");
	// Close log
	fclose(log);
	
	patricia_trie_print(p,0);
	
	return EXIT_SUCCESS;
}
