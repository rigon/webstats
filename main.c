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

#include "patricia_trie.h"

#define LINE_WIDTH	10000

typedef struct {
	char *ip;
	char *identd;
	char *user;
	char *time;
	char *request;
	char *status_code;
	char *size;
	char *referer;
	char *user_agent;
} REQUEST;


char *next_parameter(char *buf, char ending, char *last) {
	if(buf >= last) return NULL;
	
	int i=0;
	
	for(i=0; buf[i] != ending && buf[i] != '\0' && i < LINE_WIDTH; i++);
	buf[i] = '\0';
	return &buf[i+1];
}

REQUEST *process_line(char *line) {
	
	REQUEST *r = (REQUEST *)malloc(sizeof(REQUEST));
	
	char *pos = line;
	char *last = &line[LINE_WIDTH];
	
	// IP
	r->ip = pos;
	pos = next_parameter(pos, ' ', last);
	
	// identd
	r->identd = pos;
	pos = next_parameter(pos, ' ', last);
	
	// User
	r->user = pos;
	pos = next_parameter(pos, ' ', last);
	
	// Time
	pos++;	// Skip '['
	r->time = pos;
	pos = next_parameter(pos, ']', last);
	pos++;
	
	// Request
	pos++;	// Skip '"'
	r->request = pos;
	pos = next_parameter(pos, '"', last);
	pos++;
	
	// Status Code
	r->status_code = pos;
	pos = next_parameter(pos, ' ', last);
	
	// Size
	r->size = pos;
	pos = next_parameter(pos, ' ', last);
	
	// Referer
	pos++; 	// Skip '"'
	r->referer =pos;
	pos = next_parameter(pos, '"', last);
	pos++;
	
	// User Agent
	pos++; 	// Skip '"'
	r->user_agent = pos;
	pos = next_parameter(pos, '"', last);
	pos++;
	
	
	//printf("%s\nIP: %s\nIdentd: %s\nUser: %s\nTime: %s\nRequest: %s\nStatus Code: %s\nSize: %s\nReferer: %s\nUser Agent: %s\n\n",
	//	line, r->ip, r->identd, r->user, r->time, r->request, r->status_code, r->size, r->referer, r->user_agent);
	
	return r;
}



int main(int argc, char **argv) {
	PATRICIA_TRIE *p = patricia_trie_create();
	
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
		REQUEST *r = process_line(buf);
		char *k = (char *)malloc(sizeof(char)*255);
		strcpy(k, r->ip);
		patricia_trie_add(p, r->ip, k);
	
		//printf("%d\n", ++i);
	}
	
	// Close log
	fclose(log);
	
	patricia_trie_print(p,0);
	
	return EXIT_SUCCESS;
}
