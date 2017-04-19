#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "docker_launcher_enum.h"
#include "docker_launcher_connect.h"
#include "docker_launcher_json.h"

/////////////////////////////////////	Using json-c Testing
#include "json.h"
char * dockerl_json_generate_containers(char *str);
//////////////////////////////////////


#define DOCKER_SERVICE_CREATE "/usr/bin/docker service create --replicas 1"
#define DOCKER_SERVICE_UPDATE "/usr/bin/docker service update --image"
#define DOCKER_SERVICE_LIST "/usr/bin/docker service ls"
#define DOCKER_RESTART_CONTAINER "/usr/bin/docker restart"

typedef struct stContainer_s
{
	char containerID[128];
	char Image[128];
	char command[128];
	char status[128];
	char ports[128];
	char names[128];
}stcontainer;


static int checkDockerService(char *serviceName)
{
	FILE *stream = NULL;
	char buff[1024];
	int result = FALSE;

	printf("[%s][%d] check!!", __FUNCTION__, __LINE__);
	
	stream = popen(DOCKER_SERVICE_LIST, "r");

	if(stream != NULL)
	{
		while((fgets(buff, 1024, stream)))
		{
			if(strstr(buff, "ID") != NULL)
			{
				continue;
			}
			if(strstr(buff, serviceName) != NULL)
			{
				result = TRUE;
				break;
			}
		}

		pclose(stream);
	}

	return result;
}

//int dockerl_create_service(int client_fd, char *serviceName, char *registryPath, char *arg)
int dockerl_create_service(int client_fd, char *options)
{

	DOCKER *docker;
	CURLcode response;

	printf("%s", options);
#if 0
	FILE *stream = NULL;
	char service[1024];

	//if(checkDockerService(serviceName))
	//{
	//	write(client_fd, "Exists docker service !!!", strlen("Exists docker service !!!"));
	//}
	//else
	{
		
		sprintf(service, "%s %s", DOCKER_SERVICE_CREATE, options);

		stream = popen(service, "r");

		if(stream != NULL)
		{
			write(client_fd, "docker service create !!!\n", strlen("docker service create !!!\n"));
			pclose(stream);
		}
		else
		{
			write(client_fd, "docker service create error!!!\n", strlen("docker service create error!!!\n"));
		}
	}
	
	return 0;
#endif

}

int dockerl_update_image(int client_fd, char *serviceName, char *registryPath)
{
	FILE *stream = NULL;
	char service[1024];

	if(!checkDockerService(serviceName))
	{
		//dockerl_create_service(client_fd, serviceName, registryPath, "sbin/init");
		write(client_fd, "No exists docker service !", strlen("No exists docker service !"));
	}
	else
	{
		sprintf(service, "%s %s %s", DOCKER_SERVICE_UPDATE, registryPath, serviceName);
		stream = popen(service, "r");

		if(stream != NULL)
		{
			write(client_fd, "docker service update!!", strlen("docker service update!!"));
			pclose(stream);
		}
	}

	return 0;
}


int dockerl_containers_Info(int client_fd)
{
	DOCKER *docker;
	CURLcode response;
	char buf[1024];
	JSON json = {0,};
	int ret= 0;


	printf("[%s][%d] dockerl_containers_info start !!!\n", __FUNCTION__, __LINE__);
	
	docker = dockerl_docker_init();

	if(docker)
	{

		response = dockerl_docker_get(docker, "http://v1.26/containers/json?all=1");
		if(response == CURLE_OK)
		{

#if 0
			//sprintf(buf, "%d ", docker->buffer->size);
			//write(client_fd, buf, strlen(buf));			
			//write(client_fd, dockerl_docker_buffer(docker), docker->buffer->size);
			printf("dockerd SERVER-> %s\n", dockerl_docker_buffer(docker));
			dockerl_parseJSON(dockerl_docker_buffer(docker), docker->buffer->size, &json);
			printf("makeJSON Start!!\n");
			dockerl_containersInfo_makeJSON(&json, client_fd);
			dockerl_freeJSON(&json);
#else
			/////////////////////////////////////	Using json-c Testing
			{
				char *jsonbuf = NULL;

				jsonbuf = dockerl_json_generate_containers(docker->buffer->data);
				if(jsonbuf){
					printf("<dockerl_containers_Info> jObj from str:\n---\n%s\n---\n", jsonbuf);
				}
				else {
					printf("dockerl_json_generate_containers is failed................\n");
				}
				write(client_fd, jsonbuf, strlen(jsonbuf));
			}
			/////////////////////////////////////
#endif
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

	return 0;
}

int dockerl_restart_container(char * containerID)
{
	FILE *stream = NULL;
	char service[1024];

	sprintf(service , "%s %s", DOCKER_RESTART_CONTAINER, containerID);

	printf("dockerl_restart_container/n service = %s\n", service);
	
	stream = popen(service, "r");

	if(stream != NULL)
	{
		pclose(stream);
	}
	
	return 0;
}



/////////////////////////////////////	Using json-c Testing
// ToDo : free json object

// extract the json data from old json ojbect, then add this into new json object with desinged name
static void dockerl_json_add_new_from_old(json_object* jObj, const char *key, json_object* newObj, const char *newkey)
{
	struct json_object *returnObj, *tObj;
	enum json_type type;

	if(json_object_object_get_ex(jObj, key, &returnObj))
	{
		type = json_object_get_type(returnObj);


		if(type == json_type_string){			
			json_object_object_add(newObj, newkey, returnObj); 
		}
		else if(type == json_type_array){
			tObj = json_object_array_get_idx(returnObj, 0);
			type = json_object_get_type(tObj);
			if(type == json_type_string){
				json_object_object_add(newObj, newkey, tObj); 
			}
			else
				printf("%s:%d check json-object-type(%d) in key(%s)\n",  __FUNCTION__, __LINE__, type, key);
		}
		else
			printf("%s:%d check json-object-type(%d) in key(%s)\n",  __FUNCTION__, __LINE__, type, key);
	}

	return;
}

// make json data body
static struct json_object* dockerl_json_create_containers(json_object* jObj)
{
	struct json_object *newObj = json_object_new_object();

	dockerl_json_add_new_from_old(jObj, "Id", newObj, "containerId");
	dockerl_json_add_new_from_old(jObj, "Names", newObj, "containerName");
	dockerl_json_add_new_from_old(jObj, "Image", newObj, "imageName");
	dockerl_json_add_new_from_old(jObj, "State", newObj, "containerStatus");

	//printf("jObj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(newObj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

	return newObj;
}


// generate json object for csa interface
// for each command, need to change dockerl_json_create_xxx and adding header in the end of this
char * dockerl_json_generate_containers(char *str) 
{
	struct json_object *inputObj,*inputArrayObj;	// json object from input string
	struct json_object *bodyObj = NULL;		// data body json object
	struct json_object *bodyProcessingObj;	// for processing usage
	struct json_object *returnObj = NULL;	// final json object

	enum json_type type;
	char *ret_buf = NULL;

	inputObj = json_tokener_parse(str);
	//printf("inputObj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(inputObj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

	type = json_object_get_type(inputObj);

	if(type == json_type_object)
	{
	  	int index = 0;

	  	bodyObj = dockerl_json_create_containers(inputObj);
	}
	else if(type == json_type_array)
	{
		int cnt = json_object_array_length (inputObj);
		int index = 0;
		struct json_object* arrayObj;

		arrayObj = json_object_new_array();
		do{
			inputArrayObj =  json_object_array_get_idx(inputObj, index++);
			bodyProcessingObj = dockerl_json_create_containers(inputArrayObj);
			json_object_array_add (arrayObj, bodyProcessingObj);
		}while(index < cnt);

		bodyObj = arrayObj;
	}

	if(bodyObj)
	{
		returnObj = json_object_new_object();
		json_object_object_add(returnObj, "command",  json_object_new_string("GetContainersInfo"));		// add "command" object
		json_object_object_add(returnObj, "containers", bodyObj);				// add body object
		ret_buf = json_object_to_json_string_ext(returnObj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
	}

	return ret_buf;
}