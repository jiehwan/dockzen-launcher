#ifndef __DOCKER_LAUNCHER_SERVICE_H__
#define __DOCKER_lAUNCHER_SERVICE_H__

int dockerl_create_service(int client_fd, char *options);
int dockerl_update_image(int client_fd, char *serviceName, char *registryPath);
int dockerl_containers_Info(int client_fd);
int dockerl_restart_container(char * containerID);



#endif /* __DOCKER_LAUNCHER_SERVICE_H__ */
