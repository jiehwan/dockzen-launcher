#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <pthread.h>

#include "docker_launcher_statemachine.h"
#include "docker_launcher_engine.h"
#include "docker_launcher_parser.h"

#define SERVER	"/var/run/docker_launcher.sock"
#define MAXLINE 1024

int server_init(int *server_fd)
{
	int state;	
	int server_sockfd;
	struct sockaddr_un serveraddr;

	if(access(SERVER, F_OK) == 0)
	{
		unlink(SERVER);
	}
	
	if((server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error !!! \n");
		return -1;
	}	

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, SERVER);

	state = bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(state == -1)
	{
		printf("bind error !\n");
		return -1;
	}

	state = listen(server_sockfd, 5);
	if(state == -1)
	{	
		printf("listen error !!! \n");
		return -1;
	}

	(*server_fd) = server_sockfd;

}

void dockerl_engine_mainloop()
{
	int ret = 0;
	pthread_detach(pthread_self());

	printf("test!!! thread!!! OK\n");
	

	while(1)
	{
		if(getStatemachine() == STATEMACHINE_INIT)
		{
			ret = dockerl_engine_start();
			if(ret != 0)
			{
				/* Error routine*/
				setStateEvent(e_DOCKER_DAEMON_FAIL);
				continue;
			}

			ret = dockerl_swarm_active();
			
			if(ret != 0)
			{
				/* Error routine */
				setStateEvent(e_DOCKER_SWARM_FAIL);
			}

			setStateEvent(e_ENGINE_SUCCESS);
		}
		sleep(2);

	}	
	
	pthread_exit((void *)0);
	
}

void dockerl_state_mainloop(void)
{
	pthread_detach(pthread_self());

	printf("state pthread !! ok!!\n");

	dockerl_state_check();
	
	pthread_exit((void *)0);
}

int getStringSize(char * buf)
{
	int size;
	int i = 0;

	size = strlen(buf);

	for(i = 0; i<size; i++)
	{
		if(buf[i] == ' ')
			break;
	}
	i++;

	return strlen(buf+i);
	
}

int mainloop(int server_fd)
{
	struct sockaddr_un clientaddr;
	int client_len;
	pid_t pid;
	int client_sockfd;
	int string_size = 0;
	int buf_size = 0;
	
	char buf[MAXLINE];
	char *command;
	pthread_t engine_thread, state_thread;
	
	client_len = sizeof(clientaddr);
	
	pthread_create(&state_thread, NULL, &dockerl_state_mainloop, (void*)NULL);
	pthread_create(&engine_thread, NULL, &dockerl_engine_mainloop, (void*)NULL);

	
	while(1)
	{			
		
		client_sockfd = accept(server_fd, (struct sockaddr *)&clientaddr, &client_len);

		if(client_sockfd == -1)
		{
			printf("Accept error : ");
			return -1;
		}

		memset(buf, 0x00, MAXLINE);

		if(read(client_sockfd, buf, MAXLINE) <= 0)
		{
			close(client_sockfd);
			continue;
		}
		//buf[1] = '\0';
		string_size = atoi(buf);
		//buf_size = getStringSize(buf);
		buf_size = strlen(buf);

		printf("string_size = %d, buf_size = %d\n", string_size, buf_size);
		
		if(buf_size > string_size)
		{
			//additional routine
			/*
			if(read(client_sockfd, buf, MAXLINE) <= 0) 
			{
				close(client_sockfd);
				continue;
			}
			
			if(size <= 0)
			{
				close(client_sockfd);
				continue;
			}
			*/
		}
		
		//printf("buf = %s, size = %d\n", buf, size);

		if(getStatemachine() == STATEMACHINE_IDLE)
		{
			printf("SERVER-> %s\n", buf + (buf_size - string_size));
									
			dockerl_capi_mainloop(client_sockfd, buf +(buf_size - string_size));
		}
		else
		{
			write(client_sockfd, "Daemon Fail", strlen("Daemon Fail"));
		}
		//write(client_sockfd, "end", strlen("end"));
		printf("%d client byebye !!!\n", client_sockfd);
		close(client_sockfd);

		
	}

	
	return 0;
}

int main(int argc, char * argv[])
{
	int server_fd;
	struct sockaddr_un clientaddr;
	int client_len;

	printf("[docker-launcher] start !\n");

	server_init(&server_fd);

	//dockerl_state_check(server_fd);
	
	mainloop(server_fd);
	
}
