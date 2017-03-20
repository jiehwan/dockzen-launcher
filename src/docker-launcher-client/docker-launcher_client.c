#include <stdio.h>
#include <string.h>
#include "docker-launcher_client.h"

typedef enum
{
	COMMAND_CREATE = 1,
	COMMAND_UPDATE,
	COMMAND_NONE
} DOCKER_CLIENT_COMMAND;

int docker_launcher_client(char *command, char *image)
{
	int cmd = COMMAND_NONE;
	
	if(!strcmp(command, "create"))
	{
		// create service
		cmd = COMMAND_CREATE;
	}
	else if(!strcmp(command, "update"))
	{
		// update image
		cmd = COMMAND_UPDATE;
	}
	docker_launcher_client_process(image, cmd);
}


int main(int argc, char * argv[])
{

	if(argc < 2)
	{
		printf("Usage : docker-launcher-client COMMAND image\n");
		printf("Commands:\n");
		printf("    create     service create\n");
		printf("    update     image update\n");
		return 0;
	}

	docker_launcher_client(argv[1], argv[2]);
	
	
	printf("docker launcher client\n");
}
