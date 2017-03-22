#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "docker-launcher_error.h"
#include "docker-launcher_engine.h"
#include "docker-launcher_service.h"
#include "docker-launcher_process.h"
#include "docker-launcher_temp.h"


#define WA_ENABLE_ARTIK7_WIFI


int docker_launcher()
{
	int result = DOCKER_LAUNCHER_ERROR_NONE;

	//while(1)
	{
		printf("[docker_launcher] StartDockerEngine!!!\n");
		if(startDockerEngine() != DOCKER_LAUNCHER_ERROR_NONE)
			return 0;//	continue;

		//printf("[docker_launcher] CheckedDockerService!!\n");
		//if(checkDockerService() != DOCKER_LAUNCHER_ERROR_NONE)
		//	return 0;//continue;

	}
	
	return 0;
}

int main(int arc, char * argv[])
{
	printf("docker launcher\n");

#if defined(WA_ENABLE_ARTIK7_WIFI)
	check_network_connection();
#endif

	docker_launcher();

	printf("docker process\n");
	docker_launcher_process();
	
	return 0;
}
