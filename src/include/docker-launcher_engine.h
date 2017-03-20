#ifndef __DOCKER_LAUNCHER_ENGINE_H__
#define __DOCKER_LAUNCHER_ENGINE_H__




int startDockerEngine(void);
int restartDockerEngine(void);
int getProcessPid(char *str);
int getDockerdStatus(void);

#endif /*__DOCKER_LAUNCHER_ENGINE_H__*/