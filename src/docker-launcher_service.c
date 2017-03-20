#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "docker-launcher_error.h"
#include "docker-launcher_service.h"

#define DOCKER_INFO "/usr/bin/docker info"
#define DOCKER_SERVICE_LIST "/usr/bin/docker service ls"

#define DOCKER_SERVICE_CREATE "/usr/bin/docker service create --name tizen --replicas 1"
#define DOCKER_SERVICE_UPDATE "/usr/bin/docker service update --image"

#define DOCKER_SERVICE_IMG_ARGU "/sbin/init"
#define DOCKER_SERVICE_NAME	"tizen"

int checkDockerService(char *serviceName)
{
	int service_cnt = 0;
	int result = FALSE;
	FILE* stream=NULL;
	char buff[1024];

	stream = popen(DOCKER_SERVICE_LIST, "r");

	if(stream != NULL)
	{
		
		while((fgets(buff, 1024, stream)))
		{
			if(strstr(buff, "ID") != NULL)
			{
				continue;
			}
			if(strstr(buff, DOCKER_SERVICE_NAME) != NULL)
			{
				result = TRUE;
				break;
			}
		}

		pclose(stream);

	}
	
	return result;
}


int createService(char *image_name)
{
	FILE* stream = NULL;
	char service[1024];
	int result = DOCKER_LAUNCHER_ERROR_NONE;

	/* docker service create --name tizen --replicas 1 xxx sbin/init*/
	sprintf(service, "%s %s %s",DOCKER_SERVICE_CREATE, image_name, DOCKER_SERVICE_IMG_ARGU);

	printf("service is %s\n", service);
	stream = popen(service, "r");

	if(stream != NULL)
	{
		printf("[docker_launcer] docker service create!!!\n");
		pclose(stream);
	}

	return result;
}

int updateService(char *image)
{
	int result = DOCKER_LAUNCHER_ERROR_NONE;
	FILE* stream = NULL;
	char service[1024];


	if(!checkDockerService(DOCKER_SERVICE_NAME))
	{
		createService(image);
	}
	else
	{
		/* docker service update --image xxx tizen */ 
		sprintf(service, "%s %s %s", DOCKER_SERVICE_UPDATE, image, DOCKER_SERVICE_NAME);
		stream = popen(service, "r");

		if(stream != NULL)
			pclose(stream);
	}

	
	return result;
}

int getServicelistCount(int *pservice_cnt)
{
	FILE* stream=NULL;
	int service_list_cnt = 0;
	char buff[1024];
	int result = DOCKER_LAUNCHER_ERROR_NONE;

	/* docker service ls*/
	stream = popen(DOCKER_SERVICE_LIST, "r");

	if(stream != NULL)
	{
		while((fgets(buff, 1024, stream)))
		{
			if(strstr(buff, "ID") != NULL)
			{
				continue;
			}
			service_list_cnt++;
		}

		
		pclose(stream);
	}

	(*pservice_cnt) = service_list_cnt;
	
	return result;
}
#if 0
void *checkDockerServiceLoop(void *arg)
{

	pthread_detach(pthread_self());

	while(1)
	{
		
	}
}
#endif



