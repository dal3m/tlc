#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <jansson.h>

size_t 
silence(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    return size*nmemb;
}

size_t 
cb(void *ptr, size_t size, size_t nmemb, void *userdata) 
{
    FILE *output = (FILE *)userdata;
    return fwrite(ptr, size, nmemb, output);
}

CURLcode 
fetch(CURL *curl, const char *url, FILE *out) 
{
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
	return curl_easy_perform(curl);
}

json_t *
jsoninit(const char *path, const char *key)
{
	json_error_t err;
	json_t *root = json_load_file(path, 0, &err);

	if(!root) 
	{
		fprintf(stderr, "Failed to parse %s: %s at line %d\n", path, err.text, err.line);
		return NULL;
	}

	if(key) 
	{
		json_t *sub = json_object_get(root, key);
		
		if(!sub) 
		{
			fprintf(stderr, "[json] Key '%s' not found in %s\n", key, path);
			json_decref(root);
			return NULL;
		}
		
		json_incref(sub);
		json_decref(root);
		return sub;
	}

	return root;
}

#endif
