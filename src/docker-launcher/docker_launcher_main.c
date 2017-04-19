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
		printf("[%s][%d][docker-launcher] socket error !!! \n", __FUNCTION__, __LINE__);
		return -1;
	}	

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, SERVER);

	state = bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(state == -1)
	{
		printf("[%s][%d][docker-launcher] bind error !\n", __FUNCTION__, __LINE__);
		return -1;
	}

	state = listen(server_sockfd, 5);
	if(state == -1)
	{	
		printf("[%s][%d][docker-launcher] listen error !!! \n", __FUNCTION__, __LINE__);
		return -1;
	}

	(*server_fd) = server_sockfd;

}

void dockerl_engine_mainloop()
{
	int ret = 0;
	pthread_detach(pthread_self());

	printf("[%s][%d][docker-launcher] engine OK\n", __FUNCTION__, __LINE__);
	

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

	printf("[%s][%d][docker-launcher] state ok!!\n", __FUNCTION__, __LINE__);

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

#if 0
int mainloop(int server_fd)
{
	struct sockaddr_un clientaddr;
	int client_len;
	pid_t pid;
	int client_sockfd;
	int string_size = 0;	 // ex : buf -> "123 getxxxxx..." : getxxxxx total length
	int buf_size = 0;	     // ex : buf -> "123 getxxxxxxxx...." : total length
	int command_size = 0;
	int buf_size_length = 0; // ex : 123 length : 3
	
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
			printf("[%s][%d][docker-launcher]Accept error : ", __FUNCTION__, __LINE__);
			return -1;
		}

		memset(buf, 0x00, MAXLINE);

		if(read(client_sockfd, buf, MAXLINE) <= 0)
		{
			close(client_sockfd);
			continue;
		}
		
		
		string_size = atoi(buf);

		if(string_size == 0)
		{
			printf("[%s][%d] Error File Size Zero\n", __FUNCTION__, __LINE__);
			close(client_sockfd);
			continue;
		}
		
		command_size = getStringSize(buf);
		buf_size = strlen(buf);
		printf("buf = %s, buf_size = %d\n", buf, buf_size);
		
		command = (char *) malloc(string_size + 1);
		if(command == NULL)
		{
			printf("[%s][%d][docker-launcher] Fail memory allocation!!\n", __FUNCTION__, __LINE__);
			write(client_sockfd, "500", strlen("500"));
			close(client_sockfd);
			continue;
		}
		
		buf_size_length = abs(buf_size - command_size);

		sprintf(command, "%s", buf + buf_size_length);
	
		printf("buf_size_length=%d, command_size = %d, string_size = %d, command = %s\n",buf_size_length, command_size, string_size, command);
		printf("buf = %s\n", buf + buf_size_length);
		
		if(command_size < string_size)
		{
			if(read(client_sockfd, buf, string_size - command_size) <=0)
			{
				/* client error */
				write(client_sockfd, "700", strlen("700"));
				printf("[%s][%d][docker-launcher] Error client data \n", __FUNCTION__, __LINE__);
				close(client_sockfd);
				continue;
			}
			
			buf_size = strlen(buf);

			if(command_size + buf_size == string_size)
			{				
				sprintf(command + command_size, "%s", buf);				
				command_size += buf_size;				
			}
			else
			{
				printf("[%s][%d][docker-launcher] Error client data !!!!\n", __FUNCTION__, __LINE__);
				write(client_sockfd, "700", strlen("700"));
				close(client_sockfd);
				break;
			}

		}
		
		if(command_size == string_size)
		{			
			if(getStatemachine() == STATEMACHINE_IDLE)
			{
				printf("SERVER-> %s\n", command);
										
				dockerl_capi_mainloop(client_sockfd, command);
			}
			else
			{
				write(client_sockfd, "300", strlen("300"));
			}
			printf("%d client byebye !!!\n", client_sockfd);
			close(client_sockfd);
		}

		if(command != NULL)
		{
			free(command);
			command = NULL;
		}
	}
	
	return 0;
}
#else
int mainloop(int server_fd)
{
	struct sockaddr_un clientaddr;
	int client_len;
	pid_t pid;
	int client_sockfd;
	int string_size = 0;	 // ex : buf -> "123 getxxxxx..." : getxxxxx total length
	int buf_size = 0;	     // ex : buf -> "123 getxxxxxxxx...." : total length
	int command_size = 0;
	int buf_size_length = 0; // ex : 123 length : 3
	
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
			printf("[%s][%d][docker-launcher]Accept error : ", __FUNCTION__, __LINE__);
			return -1;
		}

		memset(buf, 0x00, MAXLINE);

		if(read(client_sockfd, buf, MAXLINE) <= 0)
		{
			close(client_sockfd);
			continue;
		}		
		
		printf("SERVER-> %s\n", buf);		
		
		if(getStatemachine() == STATEMACHINE_IDLE)
		{
			printf("SERVER-> %s\n", buf);
									
			dockerl_capi_mainloop(client_sockfd, buf);
		}
		else
		{
			printf("[%s][%d][docker-launcher] Fail docker-launcher\n", __FUNCTION__, __LINE__);
		}
		printf("%d client byebye !!!\n", client_sockfd);
		close(client_sockfd);
		
	}
	
	return 0;
}

#endif

int main(int argc, char * argv[])
{
	int server_fd;
	struct sockaddr_un clientaddr;
	int client_len;

	printf("[%s][%d][docker-launcher] start !\n", __FUNCTION__, __LINE__);

	server_init(&server_fd);
	
	mainloop(server_fd);
}
