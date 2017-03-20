#ifndef __DOCKER_LAUNCHER_DBUS_H__
#define __DOCKER_LAUNCHER_DBUS_H__

#include <gio/gio.h>

#define DOCKER_LAUNCHER_INTERFACE	"org.tizen.dockerlauncherd"
#define DOCKER_LAUNCHER_PATH		"unix:abstract=/org/tizen/dockerlauncherd"
#define DOCKER_LAUNCHER_SERVICE		"/org/tizen/dockerlauncherd"


const gchar docker_launcher_data_xml[] =
	"<node>"
	"  <interface name='org.tizen.dockerlauncherd'>"
	"    <method name='launcherd'>"
	"      <arg type='s' name='image_name' direction='in'/>"
	"      <arg type='i' name='command' direction='in'/>"
	"      <arg type='i' name='ret_value' direction='out'/>"
	"    </method>"
	"  </interface>"
	"</node>";

#endif /* __DOCKER_LAUNCHER_DBUS_H__ */
