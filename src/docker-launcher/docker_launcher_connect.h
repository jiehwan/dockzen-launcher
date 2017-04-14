#ifndef __DOCKER_LAUNCHER_CONNECT_H__
#define __DOCKER_LAUNCHER_CONNECT_H__

#include <curl/curl.h>

struct buffer {
  char *data;
  size_t size;
};

struct docker {
  CURL *curl;
  struct buffer *buffer;
};

typedef struct docker DOCKER;

DOCKER *dockerl_docker_init(void);
int dockerl_docker_destroy(DOCKER *docker_client);
char *dockerl_docker_buffer(DOCKER *docker_client);
CURLcode dockerl_docker_post(DOCKER *docker_client, char *url, char *data);
CURLcode dockerl_docker_get(DOCKER *docker_client, char *url);


#endif /*__DOCKER_LAUNCHER_CONNECT_H__*/

