#ifndef _LOGPARSER_INC
#define _LOGPARSER_INC


#define LINE_WIDTH	10000

typedef struct {
	char *line;
	
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


REQUEST *parse_line(char *line);
void parse_delete(REQUEST *request);


#endif /* _LOGPARSER_INC */
