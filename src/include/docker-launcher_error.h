#ifndef __DOCKER_LAUNCHER_ERROR_H__
#define __DOCKER_LAUNCHER_ERROR_H__

typedef enum
{
	DOCKER_LAUNCHER_ERROR_NONE = 0,		/* Successful */
	DOCKER_LAUNCHER_ERROR_IMAGE,		/* docker image error */
	DOCKER_LAUNCHER_ERROR_CONTAINER,	/* docker container error */
	DOCKER_LAUNCHER_ERROR_DAEMON,		/* docker daemon error*/
	DOCKER_LAUNCHER_ERROR_NETWORK,		/* network error */
	DOCKER_LAUNCHER_ERROR_SYSTEM,		/* system error */
	DOCKER_LAUNCHER_ERROR_UNKNOWN		/* unknown error */
} docker_LAUNCHER_error_e;

int Error(int err);

#define TRUE	1
#define FALSE 0
#define FILE_MAX_SIZE	1024


#endif /* __DOCKER_LAUNCHER_ERROR_H__ */
