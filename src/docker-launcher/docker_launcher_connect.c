#include <stdlib.h>
#include <string.h>

#include "docker_launcher_connect.h"

void dockerl_malloc_fail() 
{
	fprintf(stderr, "ERROR: Failed to allocate memory. Committing seppuku.");
	exit(-1);
}

static size_t dockerl_write_function(void *data, size_t size, size_t nmemb, void *buffer) 
{
	size_t realsize = size * nmemb;
	struct buffer *mem = (struct buffer *)buffer;

	mem->data = realloc(mem->data, mem->size + realsize + 1);
	if(mem->data == NULL) {
		dockerl_malloc_fail();
	}

	memcpy(&(mem->data[mem->size]), data, realsize);
	mem->size += realsize;
	mem->data[mem->size] = 0;

	return realsize;
}

void dockerl_init_buffer(DOCKER *client) 
{
	client->buffer->data = (char *) malloc(1);
	client->buffer->size = 0;
}

void dockerl_init_curl(DOCKER *client) 
{
	curl_easy_setopt(client->curl, CURLOPT_UNIX_SOCKET_PATH, "/var/run/docker.sock");
	curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, dockerl_write_function);
	curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, client->buffer);
}

CURLcode dockerl_docker_perform(DOCKER *client, char *url) 
{
	CURLcode response;
	
	dockerl_init_buffer(client);
	
	curl_easy_setopt(client->curl, CURLOPT_URL, url);

	response = curl_easy_perform(client->curl);
	curl_easy_reset(client->curl);

	return response;
}


DOCKER *dockerl_docker_init(void) 
{

	DOCKER *client;

	client = (DOCKER *) malloc(sizeof(struct docker));
	client->buffer = (struct buffer *) malloc(sizeof(struct buffer));

	client->curl = curl_easy_init();

	if (client->curl) 
	{
		dockerl_init_curl(client);
		return client;
	}

	return NULL;
}


int dockerl_docker_destroy(DOCKER *client) 
{	
  curl_easy_cleanup(client->curl);
  free(client->buffer->data);
  free(client->buffer);

  free(client);
  client = NULL;

  return 0;
}

char *dockerl_docker_buffer(DOCKER *client) 
{
  return client->buffer->data;
}

CURLcode dockerl_docker_post(DOCKER *client, char *url, char *data) 
{
	struct curl_slist *headers = NULL;
	CURLcode response;

	dockerl_init_curl(client);

	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(client->curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(client->curl, CURLOPT_POSTFIELDS, (void *)data);

	response = dockerl_docker_perform(client, url);
	curl_slist_free_all(headers);

	return response;
}

CURLcode dockerl_docker_get(DOCKER *client, char *url) 
{

	dockerl_init_curl(client);
	return dockerl_docker_perform(client, url);
}


//void 



