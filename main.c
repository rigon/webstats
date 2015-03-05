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
#include "ipinfo.h"


int main(int argc, char **argv) {
	IPINFO *info = ipinfo("66.249.78.34");
	printf("ip:%s\nhostname:%s\nloc:%s\norg:%s\ncity:%s\nregion:%s\ncontry:%s\npostal:%s\nphone:%s\n",
		   	info->ip, info->hostname, info->loc, info->org, info->city, info->region, info->country, info->postal, info->phone);
	return 0;
	//////////////////////////////
	
	
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
	PATRICIA_TRIE *p = patricia_trie_create();
	
	while( fgets(buf, LINE_WIDTH, log) != NULL ) {
		REQUEST *req = parse_line(buf);
		patricia_trie_add(p, req->ip, req->ip);
	}
	
	// Close log
	fclose(log);
	
	patricia_trie_print(p,0);
	
	return EXIT_SUCCESS;
}
