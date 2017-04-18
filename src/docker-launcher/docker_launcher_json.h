#ifndef __DOCKER_LAUNCHER_JSON_H__
#define __DOCKER_LAUNCHER_JSON_H__

#include <stdbool.h>

#define TOKEN_MAX_SIZE 1024

typedef enum _TOKEN_TYPE_e {
	TOKEN_STRING,
	TOKEN_NUMBER
} TOKEN_TYPE;

typedef struct _TOKEN_s {
	TOKEN_TYPE type;
	union 
	{
		char *string;
		double number;
	};
	bool isArray;
}TOKEN;

typedef struct _JSON_s {
	int count;
	TOKEN tokens[TOKEN_MAX_SIZE];
}JSON;

void dockerl_parseJSON(char *doc, int size, JSON *json);
char *dockerl_json_getString(JSON *json, char *key);
int dockerl_json_getNumber(JSON *json, char *key);
char *dockerl_json_getArrayString(JSON *json, char *key, int index);
void dockerl_freeJSON(JSON *json);
int dockerl_json_checksum(char *data, int size);
int dockerl_containersInfo_makeJSON(JSON *json, int client_sockfd);


#endif /* __DOCKER_LAUNCHER_JSON_H__ */
