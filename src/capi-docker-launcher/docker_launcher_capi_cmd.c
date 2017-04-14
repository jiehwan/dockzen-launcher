#include <stdio.h>
#include "docker_launcher_enum.h"
#include "docker_launcher_capi.h"

#if 0
int capi_dockerlauncher_registry_info(char *info, int size)
{
	
	return 0;
}

int capi_dockerlauncher_factory_registry_info(char *registry, int size)
{
	return 0;
}
int capi_dockerlauncher_factory_image_info(char *registry, int size)
{
	return 0;
}
int capi_dockerlauncher_service_create(char *serviceinfo, int size)
{
	return 0;
}
int capi_dockerlauncher_image_update(char *registry, int registry_size, char image, int image_size)
{
	return 0;
}
int capi_dockerlauncher_image_rollback(char *registry, int registry_size, char image, int image_size)
{
	return 0;
}
int capi_dockerlauncher_update_state()
{
	return 0;
}

int capi_dockerlauncher_container_info(container_info *info)
{
	return 0;
}
int capi_dockerluancher_container_restart(char *containerid)
{
	return 0;
}
int capi_dockerluancher_numberOfContainer(void)
{
	return 0;
}
int capi_dockerlauncher_container_log(char containerlog)
{
	return 0;
}

int capi_dockerlauncher_device_status(device_status *device)
{
	return 0;
}
int capi_dockerlauncher_device_shutdown(void)
{
	return 0;
}
int capi_dockerlauncher_device_reboot(void)
{	
	return 0;
}

int capi_dockerlauncher_docker_info(docker_info *info)
{
	return 0;
}

int capi_dockerlauncher_info(dockerlauncher_info *info)
{
	if(info != NULL)
		capi_dockerlauncher_mainLoop(CAPI_DOCKERLAUNCHER_INFO);
	
	return 0;
}

int capi_dockerlauncher_ls(void)
{
	return 0;
}
int capi_dockerlauncher_rm(void)
{
	return 0;
}
#endif
