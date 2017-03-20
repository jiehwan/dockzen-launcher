//
// Created by Daniel Suo on 2/2/17.
//
#if 0
#ifndef DOCKER_DOCKER_H
#define DOCKER_DOCKER_H

#define DOCKER_API_VERSION "v1.24"

#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"


struct buffer {
  char *data;
  int size;
};

struct docker {
  //CURL *curl;
  char *version;
  struct buffer *buffer;
};

typedef struct docker DOCKER;

DOCKER *docker_init(char *version);
//int docker_destroy(DOCKER *docker_client);
//char *docker_buffer(DOCKER *docker_client);
//int docker_post(DOCKER *docker_client, char *url, char *data);
//int docker_get(DOCKER *docker_client, char *url);


#endif //DOCKER_DOCKER_H
#endif
