#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "docker_launcher_enum.h"
#include "docker_launcher_connect.h"

#define DOCKER_SERVICE_CREATE "/usr/bin/docker service create --replicas 1"
#define DOCKER_SERVICE_UPDATE "/usr/bin/docker service update --image"
#define DOCKER_SERVICE_LIST "/usr/bin/docker service ls"
#define DOCKER_RESTART_CONTAINER "/usr/bin/docker restart"

typedef struct stContainer_s
{
	char containerID[128];
	char Image[128];
	char command[128];
	char status[128];
	char ports[128];
	char names[128];
}stcontainer;


static int checkDockerService(char *serviceName)
{
	FILE *stream = NULL;
	char buff[1024];
	int result = FALSE;

	printf("[%s][%d] check!!", __FUNCTION__, __LINE__);
	
	stream = popen(DOCKER_SERVICE_LIST, "r");

	if(stream != NULL)
	{
		while((fgets(buff, 1024, stream)))
		{
			if(strstr(buff, "ID") != NULL)
			{
				continue;
			}
			if(strstr(buff, serviceName) != NULL)
			{
				result = TRUE;
				break;
			}
		}

		pclose(stream);
	}

	return result;
}

//int dockerl_create_service(int client_fd, char *serviceName, char *registryPath, char *arg)
int dockerl_create_service(int client_fd, char *options)
{

	DOCKER *docker;
	CURLcode response;
#if 0
	FILE *stream = NULL;
	char service[1024];

	//if(checkDockerService(serviceName))
	//{
	//	write(client_fd, "Exists docker service !!!", strlen("Exists docker service !!!"));
	//}
	//else
	{
		
		sprintf(service, "%s %s", DOCKER_SERVICE_CREATE, options);

		stream = popen(service, "r");

		if(stream != NULL)
		{
			write(client_fd, "docker service create !!!\n", strlen("docker service create !!!\n"));
			pclose(stream);
		}
		else
		{
			write(client_fd, "docker service create error!!!\n", strlen("docker service create error!!!\n"));
		}
	}
	
	return 0;
#endif

}

int dockerl_update_image(int client_fd, char *serviceName, char *registryPath)
{
	FILE *stream = NULL;
	char service[1024];

	if(!checkDockerService(serviceName))
	{
		//dockerl_create_service(client_fd, serviceName, registryPath, "sbin/init");
		write(client_fd, "No exists docker service !", strlen("No exists docker service !"));
	}
	else
	{
		sprintf(service, "%s %s %s", DOCKER_SERVICE_UPDATE, registryPath, serviceName);
		stream = popen(service, "r");

		if(stream != NULL)
		{
			write(client_fd, "docker service update!!", strlen("docker service update!!"));
			pclose(stream);
		}
	}

	return 0;
}


int dockerl_containers_Info(int client_fd)
{
	DOCKER *docker;
	CURLcode response;
	char buf[1024];

	docker = dockerl_docker_init();

	if(docker)
	{

		response = dockerl_docker_get(docker, "http://v1.26/containers/json?all=1");
		if(response == CURLE_OK)
		{

			sprintf(buf, "%d ", docker->buffer->size);
			write(client_fd, buf, strlen(buf));
			write(client_fd, dockerl_docker_buffer(docker), docker->buffer->size);
			
		}
		else
		{
			write(client_fd, "0", 1);
			printf("[docker-launcher] containers info error!! \n");
		}

		dockerl_docker_destroy(docker);
	}
	else
	{
		printf("[docker-launcher] Failed to get a docker daemon \n");
	}

	return 0;
}

int dockerl_restart_container(char * containerID)
{
	FILE *stream = NULL;
	char service[1024];

	sprintf(service , "%s %s", DOCKER_RESTART_CONTAINER, containerID);

	printf("dockerl_restart_container/n service = %s\n", service);
	
	stream = popen(service, "r");

	if(stream != NULL)
	{
		pclose(stream);
	}
	
	return 0;
}