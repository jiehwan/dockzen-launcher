#ifndef __DOCKER_LAUNCHER_CAPI_H__
#define __DOCKER_LAUNCHER_CAPI_H__

typedef enum
{
	/* from daemon*/
	CAPI_REGISTRY_INFO = 0,
	CAPI_FACTORY_IMAGE_INFO,
	CAPI_UPDATE_STATE,
	CAPI_CONTAINER_INFO,
	CAPI_CONTAINER_NUMBEROFCONTAINER,
	CAPI_CONTAINER_LOG,
	CAPI_DEVICE_STATUS,
	CAPI_DOCKER_INFO,
	CAPI_DOCKERLAUNCHER_INFO,
	CAPI_SERVICE_LS,

	/* from client*/
	//CAPI_REGISTRY_INFO = 0,
	//CAPI_FACTORY_IMAGE_INFO,
	

	CAPI_END	 /* enum total number*/
}dockerlauncher_API;

#if 0
struct _dockerlauncher_info_s {
	int dockerd_status; /* 1 : running, 0 : stop*/
	int swarm_status; /* 1 : active , 0 : inactive */
};

struct _dockerlauncher_dockerinfo_s {
	char version[128];
	char storage[128];
};

struct

/* container info */
struct _dockerlauncher_container_s {
	char id[128];
	char image[128];
	char created[128];
	char status[128];
	char ports[128];
	char names[128];
};

/* container device usage */
struct _dockerlauncher_device_c_s {
	char id[128];
	int cpu;
	int mem_usage;
	int mem_limit;
	int mem;
	int netIO;
	int blockIO;
	int pids;
};



/* host device usage */
struct _dockerlauncher_device_h_s {
	char id[128];
	int cpu;
	int mem_usage;
	int mem_limit;
	int mem;
	int netIO;
	int blockIO;
	int pids;
};

/* device usage */
struct _dockerlauncher_device_s {
	container_usage container;
	host_usage host;
};


typedef struct _dockerlauncher_info_s dockerlauncher_info;
typedef struct _dockerlauncher_dockerinfo_s docker_info;
typedef struct _dockerlauncher_container_s container_info;
typedef struct _dockerlauncher_device_c_s container_usage;
typedef struct _dockerlauncher_device_h_s host_usage;
typedef struct _dockerlauncher_device_s device_status;


int capi_dockerlauncher_registry_info(char *info, int size);

int capi_dockerlauncher_factory_registry_info(char *registry, int size);
int capi_dockerlauncher_factory_image_info(char *registry, int size);
int capi_dockerlauncher_service_create(char *serviceinfo, int size);
int capi_dockerlauncher_image_update(char *registry, int registry_size, char image, int image_size);
int capi_dockerlauncher_image_rollback(char *registry, int registry_size, char image, int image_size);
int capi_dockerlauncher_update_state();

int capi_dockerlauncher_container_info(container_info *info);
int capi_dockerluancher_container_restart(char *containerid);
int capi_dockerluancher_numberOfContainer(void);
int capi_dockerlauncher_container_log(char containerlog);	


int capi_dockerlauncher_device_status(device_status *device);
int capi_dockerlauncher_device_shutdown(void);
int capi_dockerlauncher_device_reboot(void);

int capi_dockerlauncher_docker_info(docker_info *info);
int capi_dockerlauncher_info(dockerlauncher_info *info);
int capi_dockerlauncher_ls(void);
int capi_dockerlauncher_rm(void);
#endif

#endif /* __DOCKER_LAUNCHER_CAPI_H__ */
