#ifndef __DOCKER_LAUNCHER_ENGINE_H__
#define __DOCKER_LAUNCHER_ENGINE_H__

int dockerl_docker_info(int client_fd);
int dockerl_engine_start(void);
int dockerl_swarm_active(void);

#endif /* __DOCKER_LAUNCHER_ENGINE_H__ */