#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>

#include "docker_launcher_enum.h"

#define MAXLINE 1024
#define SERVER	"/var/run/docker_launcher.sock"

typedef struct stContainer_s
{
	char containerID[128];
	char Image[128];
	char command[128];
	char status[128];
	char ports[128];
	char names[128];
}stcontainer;



int dockerl_info(int client_sockfd, char *cmd)
{
	return 0;
}



void dockerl_create(int client_sockfd, char *cmd, int count, char *argv[])
{
	char cmd_line[MAXLINE];
	char *cmd_line_ptr;
	char buf_get[MAXLINE];
	int i = 0;

	cmd_line_ptr = cmd_line;
	
	sprintf(cmd_line, "%s", cmd);
	cmd_line_ptr = cmd_line_ptr + strlen(cmd);
	
	for(i = 2; i<count; i++)
	{
		//printf("argv[%d] = %s, cmd_line=%s\n", i, argv[i], cmd_line);
		sprintf(cmd_line_ptr, " %s", argv[i]);
		cmd_line_ptr = cmd_line_ptr + strlen(argv[i]) + 1;
		
	}
	
	printf("client> %s\n", cmd_line);

	
	write(client_sockfd, cmd_line, strlen(cmd_line));

	printf("server read!!\n");

	while(1)
	{
		memset(buf_get, 0x00, MAXLINE);
		read(client_sockfd, buf_get, MAXLINE);
		printf("%s", buf_get);

		if(!strncmp(buf_get + (strlen(buf_get) - strlen("end")), "end", strlen("end")))
		{
			break;
		}
	}
	
	return 0;
	
}

int dockerl_docker_info(int client_sockfd, char *cmd)
{
	char buf_get[5012];
	char buf_set[MAXLINE];
	int i;

	sprintf(buf_set, "%d ", strlen(cmd));
	write(client_sockfd, buf_set, strlen(buf_set));

	sprintf(buf_set, "%s", cmd);		
	write(client_sockfd, buf_set, strlen(buf_set));

	printf("client> %s\n", buf_set);

	printf("server read!!\n");

	while(1)
	{
		memset(buf_get, 0x00, MAXLINE);
		if(read(client_sockfd, buf_get, 5012) <= 0)
		{
			continue;
		}
		
		printf("data-> %s\n", buf_get);

		//if(!strncmp(buf_get + (strlen(buf_get) - strlen("end")), "end", strlen("end")))
		{
			break;
		}

	}

	return 0;
	
}

void dockerl_containersInfo(int client_sockfd, char *cmd)
{
	char buf_get[5012];
	char buf_set[MAXLINE];
	stcontainer container;
	int size = 0;
	int i = 0;

	sprintf(buf_set, "%d ", strlen(cmd));
	
	printf("size = %s\n", buf_set);
	write(client_sockfd, buf_set, strlen(buf_set));
	
	sprintf(buf_set, "%s", cmd);		
	write(client_sockfd, buf_set, strlen(buf_set));

	printf("client> %s\n", buf_set);

	printf("server read!!!\n");

	while(1)
	{
		memset(buf_get, 0x00, MAXLINE);

		if(read(client_sockfd, buf_get, 5012) <= 0)
		{
			continue;
		}
		
		printf("data-> %s\n", buf_get);

//		if(!strncmp(buf_get + (strlen(buf_get) - strlen("end")), "end", strlen("end")))
		{
			break;
		}
	}
	
	return 0;
	
}

void dockerl_restartContainer(int client_sockfd, char * cmd, char *containerID)
{
	char buf_get[MAXLINE];
	char cmd_line[MAXLINE];

	printf("client> %s, containerid = %s\n", cmd, containerID);

	sprintf(cmd_line, "%s %s", cmd, containerID);

	printf("cmd_line = %s\n", cmd_line);

	write(client_sockfd, cmd_line, strlen(cmd_line));
	
	while(1)
	{
		memset(buf_get, 0x00, MAXLINE);
		read(client_sockfd, buf_get, MAXLINE);
		printf("%s", buf_get);

		if(!strncmp(buf_get + (strlen(buf_get) - strlen("end")), "end", strlen("end")))
		{
			break;
		}
	}

	
	return 0;
	
}


void dockerl_update(int count, char *image)
{
}

void dockerl_ls(void)
{
}

void dockerl_rm(void)
{
}

int main(int argc, char *argv[])
{

	int client_len;
	int client_sockfd;
	FILE *fp_in;
	struct sockaddr_un clientaddr;

	if(argc < 2)
	{
		printf("Usage : dockerl COMMAND\n");
		printf("commands:\n");
		printf("	info        Display docker launcher information\n");
		printf("	dockerinfo  Display docker information\n");
		printf("    create      Create a new service \n");
		printf("	update      Update a service\n");
		printf("    ls          List services\n");
		printf("    rm          Remove one or more services\n");
		//printf("    containers  Containers info\n");
		return 0;
	}

	client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(client_sockfd == -1)
	{
		printf("client error !!\n");
		return -1;
	}

	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sun_family = AF_UNIX;
	strcpy(clientaddr.sun_path, SERVER);
	client_len = sizeof(clientaddr);

	if(connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
	{
		printf("client connect error !!\n");
		return -1;
	}

	if(!strncmp(argv[1], "info", strlen("info")))
	{
		dockerl_info(client_sockfd, "getLauncherInfo");
	}
	else if(!strncmp(argv[1], "dockerinfo", strlen("dockerinfo")))
	{
		dockerl_docker_info(client_sockfd, "getDockerInfo");
	}
	else if(!strncmp(argv[1], "create", strlen("create")))
	{
		dockerl_create(client_sockfd, "createImage", argc, argv);
	}
	else if(!strncmp(argv[1], "update", strlen("update")))
	{
		dockerl_update(argc, argv[2]);
	}
	else if(!strncmp(argv[1], "containers", strlen("containers")))
	{
		dockerl_containersInfo(client_sockfd, "getContainersInfo");
	}
	else if(!strncmp(argv[1], "restart", strlen("restart")))
	{
		dockerl_restartContainer(client_sockfd, "restartContainer", argv[2]);
	}
	else if(!strncmp(argv[1], "ls", strlen("ls")))
	{
		dockerl_ls();
	}
	else if(!strncmp(argv[1], "rm", strlen("rm")))
	{
		dockerl_rm();
	}

	close(client_sockfd);

	
	return 0;
}
