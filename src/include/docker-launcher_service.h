#ifndef __DOCKER_LAUNCHER_SERVICE_H__
#define __DOCKER_LAUNCHER_SERVICE_H__

int checkDockerService(char *serviceName);
int createService(char *image);
int updateService(char *image);

#define STRING_MAX_SIZE	1024

struct docker_service
{
	char id[STRING_MAX_SIZE];
	char name[STRING_MAX_SIZE];
	char mode[STRING_MAX_SIZE];
	char replicas[STRING_MAX_SIZE];
	char image[STRING_MAX_SIZE];
};

#endif /* __DOCKER_LAUNCHER_SERVICE_H__ */

