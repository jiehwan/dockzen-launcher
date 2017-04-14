#ifndef __DOCKER_LAUNCHER_STATEMACHINE_H__
#define __DOCKER_LAUNCHER_STATEMACHINE_H__

typedef enum
{
	STATEMACHINE_INIT = 0,
	STATEMACHINE_IDLE,
	STATEMACHINE_RECOVERY,
	STATEMACHINE_FAILED,
}e_statemachine;

typedef enum
{
	e_DOCKER_DAEMON_FAIL = 0,
	e_DOCKER_SWARM_FAIL,
	e_ENGINE_SUCCESS,
	e_CONTAINER_FAIL
}e_statemachine_event;

void setStatemachine(int state);
int getStatemachine(void);
int dockerl_state_check(void);
int setStateEvent(int event);



#endif /* __DOCKER_LAUNCHER_STATEMAHCINE_H__ */
