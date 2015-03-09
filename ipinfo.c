#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "ipinfo.h"
#include "patricia_trie.h"

#define LINE_WIDTH	500


PATRICIA_TRIE *list_ipinfo = NULL;


typedef struct {
	char *memory;
	size_t size;
} MemoryStruct;

static size_t ipinfo_WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	MemoryStruct *mem = (MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}


int fetch_json(const char *url, MemoryStruct *chunk) {
	//printf("--- %s\n",url);
	int ret = 0;
	
	CURL *curl_handle;
	CURLcode res;

	chunk->memory = malloc(1);  /* will be grown as needed by the realloc above */
	chunk->size = 0;    /* no data at this point */

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, ipinfo_WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);

	/* some servers don't like requests that are made without a user-agent
		field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	res = curl_easy_perform(curl_handle);

	/* check for errors */
	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));
		
		free(chunk->memory);
		chunk->memory = NULL;
		
		ret = 1;	// ERROR
	}
	else {
	/*
		* Now, our chunk.memory points to a memory block that is chunk.size
		* bytes big and contains the remote file.
		*
		* Do something nice with it!
		*/
		//printf("%s\n",chunk->memory);
		printf("%lu bytes retrieved\n", (long)chunk->size);
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

// 	if(chunk->memory)
// 		free(chunk->memory);

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();
	
	return ret;
}


int json_parse(const char *json, IPINFO *ipinfo) {
	json_object *json_obj = json_tokener_parse(json);
	
	enum json_type type;
	
	json_object_object_foreach(json_obj, key, val) {
		type = json_object_get_type(val);
		if(type == json_type_string) {
			const char *value = json_object_get_string(val);
			
			if(strcmp(key, "ip") == 0)
				ipinfo->ip = value;
			else if(strcmp(key, "hostname") == 0)
				ipinfo->hostname = value;
			else if(strcmp(key, "loc") == 0)
				ipinfo->loc = value;
			else if(strcmp(key, "org") == 0)
				ipinfo->org = value;
			else if(strcmp(key, "city") == 0)
				ipinfo->city = value;
			else if(strcmp(key, "region") == 0)
				ipinfo->region = value;
			else if(strcmp(key, "country") == 0)
				ipinfo->country = value;
			else if(strcmp(key, "postal") == 0)
				ipinfo->postal = value;
			else if(strcmp(key, "phone") == 0)
				ipinfo->phone = value;
		}
	}
	
	return 0;	// OK
}



IPINFO *ipinfo(const char *ip) {
	if(list_ipinfo == NULL)
		list_ipinfo = ipinfo_read("ipinfo.lst");	// Reads info from the file
	
	// Searches if the IP is already in the databse
	IPINFO *info = (IPINFO *)patricia_trie_search(list_ipinfo, ip);
	if(info != NULL) return info; // If so, returns it
	
	char url[500];
	MemoryStruct chunk;
	
	sprintf(url, IPINFO_URL, ip);
	
	// Fetch JSON
	if(fetch_json(url, &chunk) != 0)	// Error fetching JSON
		return NULL;
	
	info = (IPINFO *)calloc(1, sizeof(IPINFO));
	info->json = chunk.memory;
	
	// Parse JSON
	json_parse(chunk.memory, info);
	
	// Adds to the trie
	patricia_trie_add(list_ipinfo, info->ip, info);
	
	return info;
}

void ipinfo_print(IPINFO *info) {
	if(info != NULL)
		printf("ip:%s\nhostname:%s\nloc:%s\norg:%s\ncity:%s\nregion:%s\ncountry:%s\npostal:%s\nphone:%s\n",
			info->ip, info->hostname, info->loc, info->org, info->city, info->region, info->country, info->postal, info->phone);
}

PATRICIA_TRIE *ipinfo_read(const char *filename) {
	FILE *file = fopen(filename, "rt");
	
	if(file == NULL) {
		fprintf(stderr, "Cannot open ipinfo file '%s'...", filename);
		return NULL;
	}
	
	// Reads the log file
	char buf[LINE_WIDTH];
	PATRICIA_TRIE *p = patricia_trie_create();		// Creates a patricia_trie
	IPINFO *ipinfo = NULL;
	int param = 10;
	
	while( fgets(buf, LINE_WIDTH, file) != NULL ) {
		char *value;
		int size = strnlen(buf, LINE_WIDTH);
		
		if(size < 2)	// Empty string
			value = NULL;
		else {
			value = (char *)malloc(sizeof(char) * size);	// Allocates the value
			strncpy(value, buf, size);
			value[size-1] = '\0';
		}
		
		if(param > 9) {	// Allocates a new entry
			if(ipinfo != NULL)
				patricia_trie_add(p, ipinfo->ip, ipinfo);	// Adds the entry to the patricia_trie
			
			ipinfo = (IPINFO *)calloc(1, sizeof(IPINFO));
			param = 0;
		}
		
		switch(param) {
			case 0:
				ipinfo->ip = value;
				break;
			case 1:
				ipinfo->hostname = value;
				break;
			case 2:
				ipinfo->loc = value;
				break;
			case 3:
				ipinfo->org = value;
				break;
			case 4:
				ipinfo->city = value;
				break;
			case 5:
				ipinfo->region = value;
				break;
			case 6:
				ipinfo->country = value;
				break;
			case 7:
				ipinfo->postal = value;
				break;
			case 8:
				ipinfo->phone = value;
				break;
			case 9:
				// Empty line
				free(value);
				break;
		}
		param++;
	}
	
	// For the last entry to be added
	if(param > 9 && ipinfo != NULL) patricia_trie_add(p, ipinfo->ip, ipinfo);	// Adds the entry to the patricia_trie
	
	// Close log
	fclose(file);
	
	return p;
}


int ipinfo_write_handler(void *info, void *data) {
	FILE *file = (FILE *)info;
	IPINFO *ipinfo = (IPINFO *)data;
	
	fprintf(file, "%s\n", ipinfo->ip == NULL ? "" : ipinfo->ip);
	fprintf(file, "%s\n", ipinfo->hostname == NULL ? "" : ipinfo->hostname);
	fprintf(file, "%s\n", ipinfo->loc == NULL ? "" : ipinfo->loc);
	fprintf(file, "%s\n", ipinfo->org == NULL ? "" : ipinfo->org);
	fprintf(file, "%s\n", ipinfo->city == NULL ? "" : ipinfo->city);
	fprintf(file, "%s\n", ipinfo->region == NULL ? "" : ipinfo->region);
	fprintf(file, "%s\n", ipinfo->country == NULL ? "" : ipinfo->country);
	fprintf(file, "%s\n", ipinfo->postal == NULL ? "" : ipinfo->postal);
	fprintf(file, "%s\n", ipinfo->phone == NULL ? "" : ipinfo->phone);
	fprintf(file, "-----\n");
	
	return 0;	// SUCCESS
}


int ipinfo_write(const char *filename, PATRICIA_TRIE *patricia_trie) {
	FILE *file = fopen(filename, "wt");
	
	if(file == NULL) {
		fprintf(stderr, "Cannot write ipinfo file '%s'...", filename);
		return 1;
	}
	
	patricia_trie_iterate(patricia_trie, file, ipinfo_write_handler);
	
	// Close log
	fclose(file);
	
	return 0;
}


int ipinfo_save() {
	return ipinfo_write("ipinfo.lst", list_ipinfo);
}
