#ifndef _IPINFO_INC
#define _IPINFO_INC

// IP info provider
#define IPINFO_URL		"http://ipinfo.io/%s/json"		// %s is replaced by IP

/*	
 * Example of a request:
 * 
 * $ curl ipinfo.io/8.8.8.8
 *	{
 *		"ip": "8.8.8.8",
 *		"hostname": "google-public-dns-a.google.com",
 *		"loc": "37.385999999999996,-122.0838",
 *		"org": "AS15169 Google Inc.",
 *		"city": "Mountain View",
 *		"region": "California",
 *		"country": "US",
 *		"phone": 650
 *	}
 */

typedef struct {
	const char *ip;
	const char *hostname;
	const char *loc;
	const char *org;
	const char *city;
	const char *region;
	const char *country;
	const char *postal;
	const char *phone;
	
	char *json;
} IPINFO;

IPINFO *ipinfo(const char *ip);

#endif /* _IPINFO_INC */
