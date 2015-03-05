#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logparser.h"


char *next_parameter(char *buf, char ending, char *last) {
	if(buf > last) return last;
	
	int i;
	
	for(i=0; &(buf[i]) <= last && buf[i] != ending; i++);
	
	buf[i] = '\0';
	
	return &buf[i+1];
}


REQUEST *parse_line(char *line) {
	REQUEST *req = (REQUEST *)malloc(sizeof(REQUEST));
	
	int size = strlen(line);
	req->line = (char *)malloc(sizeof(char) * size);
	strncpy(req->line, line, size);
	
	
	char *pos = req->line;
	char *last = &(req->line[size-1]);
	
	// IP
	req->ip = pos;
	pos = next_parameter(pos, ' ', last);
	
	// identd
	req->identd = pos;
	pos = next_parameter(pos, ' ', last);
	
	// User
	req->user = pos;
	pos = next_parameter(pos, ' ', last);
	
	// Time
	pos++;	// Skip '['
	req->time = pos;
	pos = next_parameter(pos, ']', last);
	pos++;
	
	// Request
	pos++;	// Skip '"'
	req->request = pos;
	pos = next_parameter(pos, '"', last);
	pos++;
	
	// Status Code
	req->status_code = pos;
	pos = next_parameter(pos, ' ', last);
	
	// Size
	req->size = pos;
	pos = next_parameter(pos, ' ', last);
	
	// Referer
	pos++; 	// Skip '"'
	req->referer = pos;
	pos = next_parameter(pos, '"', last);
	pos++;
	
	// User Agent
	pos++; 	// Skip '"'
	req->user_agent = pos;
	pos = next_parameter(pos, '"', last);
	pos++;
	
	//printf("%s\nIP: %s\nIdentd: %s\nUser: %s\nTime: %s\nRequest: %s\nStatus Code: %s\nSize: %s\nReferer: %s\nUser Agent: %s\n\n",
	//	line, req->ip, req->identd, req->user, req->time, req->request, req->status_code, req->size, req->referer, req->user_agent);

	return req;
}

void parse_delete(REQUEST *request) {
	free(request->line);
	free(request);
}
