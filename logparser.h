/*
 * LogFormat "%v:%p %h %l %u %t \"%r\" %>s %O \"%{Referer}i\" \"%{User-Agent}i\"" vhost_combined
 * LogFormat "%h %l %u %t \"%r\" %>s %O \"%{Referer}i\" \"%{User-Agent}i\"" combined
 * LogFormat "%h %l %u %t \"%r\" %>s %O" common
 * LogFormat "%{Referer}i -> %U" referer
 * LogFormat "%{User-agent}i" agent
 */

#ifndef _LOGPARSER_INC
#define _LOGPARSER_INC

#include "ipinfo.h"

typedef struct {
	char *line;
	
	char *vhost;
	char *ip;
	char *identd;
	char *user;
	char *time;
	char *request;
	char *status_code;
	char *size;
	char *referer;
	char *user_agent;
	
	IPINFO *ipinfo;
} REQUEST;


REQUEST *parse_line(char *line);
void parse_delete(REQUEST *request);

void request_print(REQUEST *req);


#endif /* _LOGPARSER_INC */
