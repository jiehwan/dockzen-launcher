#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "docker_launcher_enum.h"
#include "docker_launcher_connect.h"

#define DOCKER_DAEMON_PATH "/usr/bin/dockerd"
#define DOCKER_PATH "/usr/bin/docker"
#define DOCKER_DAEMON_EXEC "/usr/bin/dockerd -s overlay2 &"
#define DOCKER_INFO "/usr/bin/docker info"
#define DOCKER_SWARM_INIT "/usr/bin/docker swarm init"

#define SWARM_INACTIVE	0
#define SWARM_ACTIVE	1

int checkedDockerEngineFile = FALSE;


static int checkDockerEngineFile()
{
	int result = DOCKER_LAUNCHER_ERROR_NONE;

	result = access(DOCKER_DAEMON_PATH, 0);

	if(result != DOCKER_LAUNCHER_ERROR_NONE)
	{
		printf("[docker_launcher] no exists docker file\n");
		return DOCKER_LAUNCHER_ERROR_DAEMON;
	}

	result = access(DOCKER_PATH, 0);

	if(result != DOCKER_LAUNCHER_ERROR_NONE)
	{
		printf("[docker_launcher] no exists docker file\n");
		return DOCKER_LAUNCHER_ERROR_DAEMON;
	}

	checkedDockerEngineFile = TRUE;

	return result;
}

static int startDockerd()
{
	FILE *stream = NULL;

	stream = popen(DOCKER_DAEMON_EXEC, "r");
	printf("docker_launcher] start docker daemon\n");

	if(stream != NULL)
		pclose(stream);

	sleep(2);
}

int dockerl_engine_start(void)
{
	int ret = 0;

	if(!checkedDockerEngineFile)
	{
		if(checkDockerEngineFile()!= DOCKER_LAUNCHER_ERROR_NONE)
		{
			/* no exists docker file*/
			return -1;
		}
	}

	startDockerd();

	return ret;
}

static int check_swarm()
{
	FILE *stream = NULL;
	char buff[128];
	int ret = SWARM_INACTIVE;

	stream = popen(DOCKER_INFO, "r");

	if(stream != NULL)
	{
		while(fgets(buff, 128, stream))
		{
			if(strstr(buff, "Swarm:") != NULL)
			{
				if(strstr(buff, "inactive") != NULL)
					ret= SWARM_INACTIVE;
				else
					ret = SWARM_ACTIVE;
				break;
			}
		}
		pclose(stream);
	}
	return ret;
}

static int init_swarm(void)
{
	FILE *stream;

	stream = popen(DOCKER_SWARM_INIT, "r");
	if(stream != NULL)
	{
		pclose(stream);
	}

	if(check_swarm() == SWARM_INACTIVE)
		return DOCKER_LAUNCHER_ERROR_SWARM;

	return DOCKER_LAUNCHER_ERROR_NONE;
}

int dockerl_swarm_active(void)
{
	int ret = 0;

	if(check_swarm() == SWARM_INACTIVE)
		ret = init_swarm();

	return ret;
	
}

int dockerl_docker_info(int client_fd)
{
	DOCKER *docker;
	CURLcode response;
	char buf[1024];

	docker = dockerl_docker_init();

	if(docker)
	{

		response = dockerl_docker_get(docker, "http://v1.26/info");
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
#if 0
	FILE *stream = NULL;
	char buff[1024];

	stream = popen(DOCKER_INFO, "r");

	if(stream != NULL)
	{
		while(fgets(buff, 1024, stream))
		{
			write(client_fd, buff, strlen(buff));
		}

		pclose(stream);
	}
#endif

	return 0;
}