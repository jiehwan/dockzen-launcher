#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include "docker_launcher_engine.h"
#include "docker_launcher_service.h"

int dockerlauncher_docker_info(int client_fd, char *options);
int dockerlauncher_createImage(int client_fd, char *options);
int dockerlauncher_updateImage(int client_fd, char *options);
int dockerlauncher_containersInfo(int client_fd, char *options);
int dockerlauncher_restartContainer(int client_fd, char *options);

typedef int (*cmdFunctionPtr)(int , char *);


struct _cmd_table_s
{
	char cmd[50];
	cmdFunctionPtr func;
};

typedef struct _cmd_table_s cmd_mapping_tables;

cmd_mapping_tables cmd_mapping_table[] = 
{
	{"getDockerInfo", dockerlauncher_docker_info},
	{"createImage",	dockerlauncher_createImage},
	{"updateImage",	dockerlauncher_updateImage},
	{"getContainersInfo", dockerlauncher_containersInfo},
	{"restartContainer", dockerlauncher_restartContainer},
	{"end", NULL}
};

int dockerlauncher_docker_info(int client_fd, char *options)
{
	/* docker info */
	dockerl_docker_info(client_fd);	
}


int dockerlauncher_createImage(int client_fd, char *options)
{
	
	printf("options = %s !!\n", options);
	dockerl_create_service(client_fd, options); //"tizen", "13.124.64.10:80/update_test", "/sbin/init");
}

int dockerlauncher_updateImage(int client_fd, char *options)
{
	dockerl_update_image(client_fd, "tizen", "13.124.64.10:80/update_test");
}

int dockerlauncher_containersInfo(int client_fd, char *options)
{
	printf("containers!!!\n");
	dockerl_containers_Info(client_fd);
}

int dockerlauncher_restartContainer(int client_fd, char *options)
{
	printf("container ID = %s\n", options);
	dockerl_restart_container(options);

	write(client_fd, "restart !!!", strlen("restart !!!"));
}


int dockerl_capi_mainloop(int client_fd, char *cmd)
{
	int i = 0;
	char *options = NULL;

	printf("cmd = %s\n", cmd);

	while(1)
	{
		if(!strncmp("end", cmd_mapping_table[i].cmd, strlen("end")))
		{
			break;
		}

		if(!strncmp(cmd_mapping_table[i].cmd, cmd, strlen(cmd_mapping_table[i].cmd)))
		{
			
			if(strlen(cmd) > strlen(cmd_mapping_table[i].cmd))
			{
				options = cmd + strlen(cmd_mapping_table[i].cmd) + 1;
			}
			
			cmd_mapping_table[i].func(client_fd, options);
			break;
		}

		i++;
		
	}
}

