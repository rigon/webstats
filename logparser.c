#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __USE_XOPEN
#include <time.h>

#include "logparser.h"


char *next_parameter_time(char *t, char ending, char *last) {
	if(t > last) return last;
	
	//struct tm tm; memset(&tm, 0, sizeof(struct tm));
	//strptime(req->time, "%d/%b/%Y:%H:%M:%S", &tm);
	//strftime(req->time, pos-req->time-2, "%Y%m%d_%H%M%S", &tm);
	
	/*
	 *  1    2    3    4    5    6    7    8    9    10   11   12
	 * Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
	 * 
	 * 0000000000111111111122222222
	 * 0123456789012345678901234567
	 * [22/Feb/2015:06:27:47 +0000]
	 */
	int m=0;
	switch(t[4]) {
		case 'A': switch(t[5]) { case 'p': m=4; break; case 'u': m=8; } break;
		case 'D': m=12; break;
		case 'F': m=2; break;
		case 'J': if(t[5] == 'a') m=1; else switch(t[6]) { case 'n': m=6; break; case 'l': m=7; } break;
		case 'M': switch(t[6]) { case 'r': m=3; break; case 'y': m=5; } break;
		case 'N': m=11; break;
		case 'O': m=10; break;
		case 'S': m=9; break;
		default: fputs("Wrong month format", stderr);
	}
	
	// Day
	t[6]=t[1];t[7]=t[2];
	
	// Month
	t[4]=(m<10 ? '0' : '1'); t[5]='0' + m%10;
	
	// Year
	t[0]=t[8];t[1]=t[9];t[2]=t[10];t[3]=t[11];
	
	// Hour
	t[8]=t[13];t[9]=t[14];
	
	// Minute
	t[10]=t[16];t[11]=t[17];
	
	// Second
	t[12]=t[19];t[13]=t[20];
	
	// Timezone
	t[14]=' ';t[15]=t[22];t[16]=t[23];t[17]=t[24];t[18]=t[25];t[19]=t[26];
	
	t[20] = '\0';
	
	return &t[28];
}

char *next_parameter(char *buf, char ending, char *last) {
	if(buf > last) return last;
	
	int i;
	
	for(i=0; &(buf[i]) <= last && buf[i] != ending; i++);
	
	buf[i] = '\0';
	
	return &buf[i+1];
}


REQUEST *parse_line(char *line) {
	if(line == NULL) return NULL; 
	if(line[0] == '\0') return NULL;	// Empty line
	
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
	//pos++;	// Skip '['
	req->time = pos;
	pos = next_parameter_time(pos, ']', last);
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
	
	return req;
}

void parse_delete(REQUEST *request) {
	free(request->line);
	free(request);
}

void request_print(REQUEST *req) {
	if(req != NULL)
		printf("%s\nIP: %s\nIdentd: %s\nUser: %s\nTime: %s\nRequest: %s\nStatus Code: %s\nSize: %s\nReferer: %s\nUser Agent: %s\n\n",
		req->line, req->ip, req->identd, req->user, req->time, req->request, req->status_code, req->size, req->referer, req->user_agent);
}
