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

int capi_dockerlauncher_mainLoop(int api_num, char *info_data)
{
	int client_len;
	int client_sockfd;
	FILE *fp_in;
	char buf_in[MAXLINE];
	char buf_get[MAXLINE];
	struct sockaddr_un clientaddr;

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

	while(1)
	{
		memset(buf_in, 0x00, MAXLINE);
		memset(buf_get, 0x00, MAXLINE);
		printf("client> ");
		fgets(buf_in, MAXLINE, stdin);
		write(client_sockfd, buf_in, strlen(buf_in));
		read(client_sockfd, buf_get, MAXLINE);
		printf("client-> %s", buf_get);
	}

	close(client_sockfd);
	return 0;
	
}
