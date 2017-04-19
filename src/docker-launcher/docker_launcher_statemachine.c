#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include "docker_launcher_enum.h"
#include "docker_launcher_statemachine.h"

#define DOCKER_DAEMON_NAME "dockerd"

int dockerlauncher_state = STATEMACHINE_INIT;


static int which_number(char *s)
{
	int len, i;
	len = strlen(s);

	for(i = 0; i<len; i++)
		if((s[i] < '0' || s[i] > '9'))
			return -1;

	return atoi(s);
}

static int getProcessPid(char *str)
{

	DIR *dp;
	struct dirent *dir;
	char buf[100], line[1024], tag[100], name[100];
	int pid;
	FILE *fp;

	dp = opendir("/proc");

	if(!dp)
	{
		printf("[%s][%d][docker-launcher] proc error !!\n", __FUNCTION__, __LINE__);
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

static int checkDockerdState(void)
{
	int pid = 0;

	pid = getProcessPid(DOCKER_DAEMON_NAME);

	if(pid == -1)
		return FALSE;

	return TRUE;
}


void setStatemachine(int state)
{
	dockerlauncher_state = state;
}

int getStatemachine(void)
{
	return dockerlauncher_state;
}

static int transition(int state)
{
	setStatemachine(state);
	
	return 0;
}

int setStateEvent(int event)
{
	
	switch(event)
	{
	case e_DOCKER_DAEMON_FAIL:
	case e_CONTAINER_FAIL:
	case e_DOCKER_SWARM_FAIL:
		transition(STATEMACHINE_RECOVERY);
		transition(STATEMACHINE_RECOVERY);
		break;
	case e_ENGINE_SUCCESS:
		transition(STATEMACHINE_IDLE);
		break;	
	}

	return 0;
}

int dockerl_state_check(void)
{
	setStatemachine(STATEMACHINE_INIT);


	while(1)
	{
		switch(getStatemachine())
		{
		case STATEMACHINE_INIT:
			break;
		case STATEMACHINE_IDLE:

			if(checkDockerdState() != TRUE)
				setStateEvent(e_DOCKER_DAEMON_FAIL);
			
			break;
		case STATEMACHINE_RECOVERY:
			printf("RECOVERY!!!\n");
			transition(STATEMACHINE_INIT);
			break;
		case STATEMACHINE_FAILED:
			break;
		}

		sleep(2);
	}
	return 0;
}


