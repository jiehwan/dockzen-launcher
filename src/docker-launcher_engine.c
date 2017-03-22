#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include "docker-launcher_error.h"

#define DOCKER_DAEMON_PATH "/usr/bin/dockerd"
#define DOCKER_PATH	"/usr/bin/docker"
#define DOCKER_DAEMON_EXEC "/usr/bin/dockerd -s overlay &"
#define DOCKER_INFO "/usr/bin/docker info"
#define DOCKER_DAEMON_NAME "dockerd"
#define DOCKER_DAEMON_KILL "/usr/bin/kill -9 "

#define DOCKER_SWARM_INIT "/usr/bin/docker swarm init"
#define SWARM_ACTIVE	1
#define SWARM_INACTIVE	0

int dockerd_running_status = FALSE;

static int initSwarm(void)
{
	FILE *stream = NULL;
	char buff[1024];
	int swarmflag = SWARM_INACTIVE;

	printf("[docker_launcher]swarm init\n");
	
	stream = popen(DOCKER_INFO, "r");

	if(stream != NULL)
	{
		while(fgets(buff, 1024, stream))
		{
			if(strstr(buff, "Swarm:") != NULL)
			{			
				if(strstr(buff, "inactive") != NULL)
				{
					/* swarm inactive */
					swarmflag = SWARM_INACTIVE;
					printf("[docker_launcher] swarm inactive!!\n");
				}
				else
				{
					swarmflag = SWARM_ACTIVE;
					printf("[docker_launcher] swarm active!!\n");
				}

				break;
			}
		}
		pclose(stream);
		
		if(swarmflag == SWARM_INACTIVE)
		{		
			/* swarm init */
			stream = popen(DOCKER_SWARM_INIT, "r");
			if(stream != NULL)
			{
				//swarmflag = SWARM_ACTIVE;
				pclose(stream);
			}
			else
				printf("[docker_launcher] Cannot connect wifi or ethernet\n");

			sleep(1);
		}
	}
	else
	{		
		printf("[docker_launcher] Cannot connect to the docker daemon\n"); 
	}

	return swarmflag;

}


static int checkDockerEngine(void)
{

	int result = DOCKER_LAUNCHER_ERROR_NONE;

	result = access(DOCKER_DAEMON_PATH, 0);

	if(result != DOCKER_LAUNCHER_ERROR_NONE)
	{
		printf("[docker_launcher] docker engine Error\n");
		return DOCKER_LAUNCHER_ERROR_DAEMON;
	}

	result = access(DOCKER_PATH, 0);

	if(result != DOCKER_LAUNCHER_ERROR_NONE)
	{
		printf("[docker_launcher] docker engine Error\n");
		return DOCKER_LAUNCHER_ERROR_DAEMON;
	}
	
	return result;
	
}

static int which_number(char *s)
{
	int len, i;

	len = strlen(s);

	for(i=0;i<len; i++)
		if((s[i] < '0' || s[i] > '9'))
			return -1;

	return atoi(s);
}

int getProcessPid(char *str)
{
	DIR *dp;
	struct dirent *dir;
	char buf[100], line[1024], tag[100], name[100];
	int pid;
	FILE *fp;

	dp = opendir("/proc");
	if(!dp)
	{
		printf("proc error !!\n");
		return -1;
	}

	while((dir = readdir(dp)))
	{
		pid = which_number(dir->d_name);
		if(pid == -1)
			continue;

		/* Open /proc/pid/status file */
		snprintf(buf, 100, "/proc/%d/status", pid);
		fp = fopen(buf, "r");
		if(fp == NULL)
			continue;

		/* Get first line with name*/
		fgets(line, 1024, fp);

		/* Close stream*/
		fclose(fp);

		sscanf(line, "%s %s", tag, name);
		if(!strcmp(name, str))
		{
			closedir(dp);
			return pid;
		}
	}

	closedir(dp);
	return -1;
}

int checkDockerd(void)
{
	FILE * stream;
	char buff[FILE_MAX_SIZE];
	int pid = -1;
	
	stream = popen(DOCKER_INFO, "r");

	if(stream != NULL)
	{
		if(fgets(buff, FILE_MAX_SIZE, stream) > 0)
		{
			pid = getProcessPid(DOCKER_DAEMON_NAME);
		}

		pclose(stream);
	}
	return pid;
}

void *checkDockerdLoop(void *arg)
{
	FILE *stream = NULL;
	pthread_detach(pthread_self());

	while(1)
	{
		if(checkDockerd() == -1)
		{
			dockerd_running_status = FALSE;
			stream = popen(DOCKER_DAEMON_EXEC, "r");
			printf("[docker_launcher] Start Docker daemon\n");

			if(stream != NULL)
				pclose(stream);

			sleep(5);
			dockerd_running_status = TRUE;
			
		}
		sleep(2);
		
	}

	pthread_exit((void *) 0);
	
}

void *checkSwarmLoop(void)
{
	pthread_detach(pthread_self());

	while(1)
	{
	//	if(checkDockerd() != -1)
	//		break;
		if(dockerd_running_status == TRUE)
			break;
		
		sleep(1);
	}
	
	sleep(5);

	printf("[docker-launcher] dockerd_running_status = %d\n", dockerd_running_status);
	
	while(1)
	{
		if(initSwarm() == SWARM_ACTIVE)
			break;
	}

	pthread_exit((void *) 0);

}


int getDockerdStatus()
{
	printf("GetDockerdStatus = %d\n", dockerd_running_status);
	return dockerd_running_status;
}

int startDockerd(void)
{
	pthread_t threads;
	
	pthread_create(&threads, NULL, &checkDockerdLoop, (void*)NULL);
#if 0
	FILE *stream = NULL;

	stream = popen(DOCKER_DAEMON_EXEC, "r");
	printf("[docker_launcher] Start Docker daemon\n");			

	if(stream != NULL)
		pclose(stream);	
#endif
}



int startSwarm(void)
{	
	pthread_t threads;
	printf("[docker-launcher] SWARM INIT!!!!!!\n");
	pthread_create(&threads, NULL, &checkSwarmLoop, (void*)NULL);
}

int startDockerEngine(void)
{

	int dockerd_start = FALSE;
	
	if(checkDockerEngine() != DOCKER_LAUNCHER_ERROR_NONE)
	{
	 /* no exists docker file */
		return -1;
	}

	startDockerd();

	printf("[docker-launcher] startSWARM !!!\n");
	startSwarm();

	return 0;
}


int restartDockerEngine(void)
{
	int pid = 0;
	FILE* stream = NULL;
	char buff[1024];
	int len = 0;
	int dockerd_start = FALSE;

	pid = getProcessPid(DOCKER_DAEMON_NAME);

	if(pid != -1)
	{	
		sprintf(buff, "%s%d", DOCKER_DAEMON_KILL, pid);

		stream = popen(buff, "r");
		if(stream != NULL)
			pclose(stream);

	}

	while(checkDockerd() == -1)
	{

		if(dockerd_start == FALSE)
		{
			startDockerd();		
			dockerd_start = TRUE;
		}
		
		sleep(2);
	}
	
	return 0;
}


