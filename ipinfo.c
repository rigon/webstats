#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

#include "ipinfo.h"


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
		return 1;	// ERROR
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
	
	return 0;	// OK
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
	IPINFO *ipinfo = (IPINFO *)calloc(1, sizeof(IPINFO));
	char url[500];
	MemoryStruct chunk;
	
	sprintf(url, IPINFO_URL, ip);
	
	// Fetch JSON
	fetch_json(url, &chunk);
	ipinfo->json = chunk.memory;
	
	// Parse JSON
	json_parse(chunk.memory, ipinfo);
	
	return ipinfo;
}


