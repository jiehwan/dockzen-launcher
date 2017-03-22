#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tizen.h>

#include "docker-launcher_temp.h"

static void WiFiScanCallback(wifi_error_e err, void* data)
{
	printf("[docker_launcher] wifi_test-WiFiScanCallback\n");
    if(data != NULL) {
        printf("[docker_launcher] wifi_test-WiFiScanCallback data{%s}\n", data);
    }
}

static void WiFiActivateCallback(wifi_error_e result, void* user_data)
{
	int ret;

    printf("[docker_launcher] wifi_test-WiFiActivateCallback\n");
    ret = wifi_scan(WiFiScanCallback, NULL);
    if (ret != WIFI_ERROR_NONE) {
    	printf("[docker_launcher] wifi_test-WiFiActivateCallback Error!!!\n");
    }
}

/* temporary~~ check connecton 
*	check wifi in case of ARTIK7
*	1. start wlan.sh 
*	2. activate wifi
*	it should be guaranteed in init process. but ARTIK7 has some issue.
*	additionally, swarm needs to check network connection to activate initial mode
*/

static void wifi_test(void)
{
	int ret;
	bool wifiState = false;
	FILE *stream = NULL;

	printf("wlan.sh start~~~\n");
	//execv("wlan.sh","start");

	stream = popen(WLAN_EXEC, "r");
	if(stream != NULL)
	{
		int retry_count=0;
		bool initizlized = false;
		do
		{
			sleep(1);

			if(initizlized == false)
			{
				ret = wifi_initialize();
			}
			else
			{
				ret = WIFI_ERROR_NONE;
			}

			if (WIFI_ERROR_NONE != ret) {
				printf("wifi_initialize() Fail(%d)\n", ret);
			}
			else
			{
				initizlized = true;

				wifi_is_activated(&wifiState);
				printf("[docker_launcher] wifi_test-activated state(%d)\n", wifiState);

				if (wifiState == false) {
					printf("[docker_launcher] wifi_test-wifi do activate~~~\n");
			        ret = wifi_activate(WiFiActivateCallback, NULL);

			        if(ret != WIFI_ERROR_NONE) {
			            printf("[docker_launcher] wifi_test-activate error\n");
			        }
			    }
			}
		}while(false==wifiState && retry_count++ < 10);
	}
	else
	{
		printf("[docker_launcher] wlan start error\n");
	}
}

void check_network_connection(void)
{
	wifi_test();
}

