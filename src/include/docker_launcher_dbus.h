#ifndef __DOCKER_LAUNCHER_DBUS_H__
#define __DOCKER_LAUNCHER_DBUS_H__

#include <gio/gio.h>

#define DOCKER_LAUNCHER_INTERFACE	"org.tizen.dockerlauncherd"
#define DOCKER_LAUNCHER_PATH		"unix:abstract=/org/tizen/dockerlauncherd"
#define DOCKER_LAUNCHER_SERVICE		"/org/tizen/dockerlauncherd"

#define DAEMON_METHOD_NAME "from_daemon"
#define CLIENT_METHOD_NAME "from_client"


const gchar docker_launcher_data_xml[] =
	"<node>"
	"  <interface name='org.tizen.dockerlauncherd'>"
	"    <method name='from_daemon'>"
	"      <arg type='i' name='api_num' direction='in'/>"
	"      <arg type='s' name='info' direction='out'/>"
	"      <arg type='i' name='ret_value' direction='out'/>"
	"    </method>"	
	"    <method name='from_client'>"
	"      <arg type='i' name='api_num' direction='in'/>"
	"      <arg type='s' name='info' direction='in'/>"
	"      <arg type='s' name='info' direction='out'/>"
	"      <arg type='i' name='ret_value' direction='out'/>"
	"    </method>"
	"  </interface>"
	"</node>";


#endif /* __DOCKER_LAUNCHER_DBUS_H__ */
