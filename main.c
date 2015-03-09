#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logparser.h"
#include "patricia_trie.h"
#include "ipinfo.h"

#define LINE_WIDTH	10000

int ips = 0;

int processip(void *info, void *data) {
	REQUEST *req = (REQUEST *)data;
	
	//printf("%d: %s\n", ++ips, req->ip);
	IPINFO *ip = ipinfo(req->ip);
	req->ipinfo = ip;
	
	return 0;
}


// int print(void *info, void *data) {
// 	REQUEST *d = (REQUEST *)data;
// 	
// 	printf("%s\n",d->ip);
// 	return 0;
// }

int main(int argc, char **argv) {
	
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
	
	
	printf("Reading log...\t");
	// Reads the log file
	char buf[LINE_WIDTH];
	PATRICIA_TRIE *p = patricia_trie_create();
	while( fgets(buf, LINE_WIDTH, log) != NULL ) {
		REQUEST *req = parse_line(buf);
		if(req != NULL) patricia_trie_add(p, req->ip, req);
	}
	// Close log
	fclose(log);
	puts("[ OK ]");
	
	//patricia_trie_iterate(p, NULL, print);
	//patricia_trie_print(p,0);
	
	printf("Processing IPs...\t");
	patricia_trie_iterate(p, NULL, processip);
	puts("[ OK ]");
	
	printf("Saving Info...\t");
	ipinfo_save();
	puts("[ OK ]");
	
	return EXIT_SUCCESS;
}
