#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "docker_launcher_json.h"

/* JSON -> struct */
void dockerl_parseJSON(char *doc, int size, JSON *json)
{
	int tokenIndex = 0; 
	int pos = 0; // string position.
	char *begin, *end;
	int stringLength;
	char *buffer;
	bool flag = false;

	if(doc[pos] == '[')
		pos++;
		
		
	if(doc[pos] != '{') // first string {
	{
		printf("first string is not {\n");
		return ;
	}
	
	if(doc[pos] == '{')
		pos++;

	//pos++;
	printf("dockerl_parseJSON\n");

	while(pos < size)
	{
		switch(doc[pos])
		{
		case '"':
			begin = doc + pos + 1; // first position
			end = strchr(begin, '"'); // end position
			if(end == NULL)
				break;

			stringLength = end - begin; 

			json->tokens[tokenIndex].type = TOKEN_STRING;
			json->tokens[tokenIndex].string = malloc(stringLength + 1);
			memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

			memcpy(json->tokens[tokenIndex].string, begin, stringLength);

			//printf("json->token[%d] %s\n", tokenIndex, json->tokens[tokenIndex].string);

			tokenIndex++;

			pos = pos + stringLength + 1;
			
			break;
		case '[':
			/* test code [workaround] */
			pos++;

			while(doc[pos] != ']')
			{

				if(doc[pos] == '"')
				{	
					char *begin = doc + pos +1;
					char *end = strchr(begin, '"');
					if(end == NULL)
						break;

					stringLength = end - begin;

					json->tokens[tokenIndex].type = TOKEN_STRING;
					json->tokens[tokenIndex].string = malloc(stringLength+1);
					json->tokens[tokenIndex].isArray = true;

					memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

					memcpy(json->tokens[tokenIndex].string, begin, stringLength);
					tokenIndex++;
					pos = pos + stringLength + 1;
				}
				else
				{	
					flag = true;
				}

				pos++;
			}
			if(flag == true)
			{
				json->tokens[tokenIndex].type = TOKEN_STRING;
				json->tokens[tokenIndex].string = malloc(strlen("TEST") +1 );
				memcpy(json->tokens[tokenIndex].string, "TEST", strlen("TEST"));
				tokenIndex++;
			}
			break;
			
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case '-':
			begin = doc + pos;
			end = strchr(doc + pos, ',');
			if(end == NULL)
			{
				end = strchr(doc + pos, '}');
				if(end == NULL)
					break;
			}

			stringLength = end - begin;
			
//			buffer = malloc(stringLength + 1);
//			memset(buffer, 0, stringLength +1);

			json->tokens[tokenIndex].type = TOKEN_STRING;
			json->tokens[tokenIndex].string = malloc(stringLength+1);
			memset(json->tokens[tokenIndex].string, 0, stringLength + 1);
			memcpy(json->tokens[tokenIndex].string, begin, stringLength);

			//printf("stringLength = %d,end = %s, begin = %s, buffer=%s, %d \n", stringLength, end, begin, buffer, atoi(buffer));

			
			//json->tokens[tokenIndex].number = atoi(buffer);
			 //= buffer;
			

			//free(buffer);

			//printf("json->token[%d] %d\n", tokenIndex, json->tokens[tokenIndex].number);
			tokenIndex++;
			pos = pos + stringLength + 1;
			break;
		}
		pos++;
	}
	json->count = tokenIndex;
	printf("dockerl_parseJSON END count = %d\n", json->count/*TOKEN_MAX_SIZE*/);
}

char *dockerl_json_getString(JSON *json, char *key)
{
	int i = 0;
	
	for(i=0; i< json->count/*TOKEN_MAX_SIZE*/; i++)
	{
//		printf(">> %s\n", json->tokens[i].string);
	
		if((json->tokens[i].type == TOKEN_STRING) && strlen(json->tokens[i].string) == strlen(key) && !strcmp(json->tokens[i].string, key))
		{
			if(json->tokens[i+1].type == TOKEN_STRING)
			{
				printf("buf = %s\n", json->tokens[i+1].string);
				return json->tokens[i+1].string;
			}
		}
	
		
	}
	
	printf("buf = NULL\n");


	return NULL;
}

/* [workaround] test code*/
char *dockerl_json_getStringCnt(JSON *json, char *key)
{
	int i = 0;
	int count = 0;
	
	for(i=0; i< json->count/*TOKEN_MAX_SIZE*/; i++)
	{
		if((json->tokens[i].type == TOKEN_STRING) && !strncmp(json->tokens[i].string, key, strlen(key)))
		{
			if(json->tokens[i+1].type == TOKEN_STRING)
				count++;
				//return json->tokens[i+1].string;
		}
			
	}

	return count;

}

int dockerl_json_getNumber(JSON *json, char *key)
{
	int i = 0;

	printf("TOKEN_MAX_SIZE = %d\n", TOKEN_MAX_SIZE);
	
	for(i = 0; i<json->count/*TOKEN_MAX_SIZE*/; i++)
	{
		if((json->tokens[i].type == TOKEN_STRING) && !strcmp(json->tokens[i].string, key))
		{
			if(json->tokens[i+1].type == TOKEN_NUMBER)
				return json->tokens[i+1].number;
		}
	}

	return 0;
}

char *dockerl_json_getArrayString(JSON *json, char *key, int index)
{
	int i = 0;
	
	for(i = 0; i< json->count/*TOKEN_MAX_SIZE*/; i++)
	{
		if(json->tokens[i].type == TOKEN_STRING && !strcmp(json->tokens[i].string, key))
		{
			if(json->tokens[i + 1 + index].type == TOKEN_STRING && json->tokens[i + 1 + index].isArray == true)
				return json->tokens[i + 1 + index].string;
		}
	}

	return NULL;
}

int dockerl_json_getArrayCount(JSON *json, char *key)
{
	int i = 0;
	int j = 0;
	
	for(i = 0; i< json->count/*TOKEN_MAX_SIZE*/; i++)
	{
		if(json->tokens[i].type == TOKEN_STRING && !strcmp(json->tokens[i].string, key))
		{
			j = 0;
			while(json->tokens[i + 1 + j].isArray == true)
				j++;

			return j;
		}
	}

	return 0;
}

void dockerl_freeJSON(JSON *json)
{
	int i = 0;
	for(i =0; i< json->count/*TOKEN_MAX_SIZE*/; i++)
	{
		if(json->tokens[i].type == TOKEN_STRING)
		{
			free(json->tokens[i].string);
		}
	}
}

int dockerl_json_checksum(char *data, int size)
{
	int i;
	int start_flag = 0;
	int array_flag = 0;

	size = strlen(data);

	for(i = 0; i<size; i++)
	{
		switch(data[i])
		{
		case '{':
			start_flag +=1;
			break;
		case '}':
			start_flag -=1;
			break;
		case '[':
			array_flag += 1;
			break;
		case ']':
			array_flag -= 1;
			break;			
		}
	}

	if(start_flag == 0 && array_flag == 0)
		return 0;
	
	return -1;
}

void dockerl_json_getTotalString(JSON *json, char *key, char *buf[])
{
	int i = 0;
	int j = 0;
	
	for(i=0; i< json->count; i++)
	{
		printf(">> %s\n", json->tokens[i].string);
		#if 0
		if((json->tokens[i].type == TOKEN_STRING) && !strncmp(json->tokens[i].string, key, strlen(key)))
		{
			if(strlen(json->tokens[i].string) == strlen(key))
			{
				if(json->tokens[i+1].type == TOKEN_STRING)
				{
					sprintf(buf[j], "%s", json->tokens[i+1].string);
					printf("buf = %s\n", buf[j]);
					j++;
				}
			}
			else 
				printf("string => %s\n", json->tokens[i].string);
		}
		#endif
		
	}

	printf("totalString End!!\n");

}


/* [workaround] container info test code*/
int dockerl_containersInfo_makeJSON(JSON *json, int client_sockfd)
{
	int containers_cnt = 0;
	char buf[1024];
	int i;
	char containerName[10][128];
	char imageName[10][128];
	char containerStatus[10][128];

	printf("dockerl_containersInfo_makeJSON!!\n");
	containers_cnt = dockerl_json_getStringCnt(json, "Id");
	printf("containers_cnt = %d\n");

	sprintf(buf, "{\"command\":\"GetContainersInfo\",");
	printf("SERVER json >> %s", buf);

	write(client_sockfd, buf, strlen(buf));
	sprintf(buf, "\"containers\":[");
	write(client_sockfd, buf, strlen(buf));
	printf("%s", buf);
//	dockerl_json_getTotalString(json, "Names", containerName);
//	dockerl_json_getTotalString(json, "Image", imageName);
	//dockerl_json_getTotalString(json, "State", containerStatus);


//	for(i = 0; i<containers_cnt; i++)
	{
	#if 0
		printf(">> %s\n", containerName[i]);
		
		sprintf(buf, "{\"containerName\":\"");
		write(client_sockfd, buf, strlen(buf));
		
		sprintf(buf, "%s",dockerl_json_getString(json, "Names"));
		write(client_sockfd, buf, strlen(buf));
		//dockerl_json_getTotalString(json, "Names", containerName);
		

	
		sprintf(buf, "\",\"imageName\":\"");
		write(client_sockfd, buf, strlen(buf));
		printf("%s", buf);

		sprintf(buf, "13.124.64.10:80/test");//,//dockerl_json_getString(json, "Image"));
		write(client_sockfd, buf, strlen(buf));
		printf("%s", buf);
		
		sprintf(buf, "\",\"containerStatus\":\"");
		write(client_sockfd, buf, strlen(buf));
		printf("%s", buf);

		sprintf(buf, "Exited");//dockerl_json_getString(json, "State"));
		write(client_sockfd, buf, strlen(buf));
		printf("%s", buf);

		sprintf(buf, "\"}");
		write(client_sockfd, buf, strlen(buf));
		printf("%s", buf);
#endif
	   write(client_sockfd, "{\"containerID\":\"tizen\",\"imageName\":\"13.123.123\",\"containerStatus\":\"Exited\"}", strlen("{\"containerID\":\"tizen\",\"imageName\":\"13.123.123\",\"containerStatus\":\"Exited\"}"));
		
		//if(i+1 < containers_cnt)
		//{
		//	write(client_sockfd, ",", strlen(","));
		//	printf(",");
		//}
	}
	
	sprintf(buf, "]}");
	write(client_sockfd, buf, strlen(buf));
	printf("%s\n", buf);

	return 0;
	
}
#if 0
int parseJSON(char *doc)
{
	JSON json= {0,};
	int size = 0;
	char *doc;

	if(doc == NULL)
		return -1;

	size = strlen(doc);
	
	parseJSON(doc, size, &json);

	freeJSON(&json);

	return 0;
	
}
#endif
