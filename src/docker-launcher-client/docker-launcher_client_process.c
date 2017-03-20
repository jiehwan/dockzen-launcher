#include <stdio.h>
#include "docker-launcher_dbus.h"

GDBusConnection *create_connection = NULL;
#define METHOD_NAME "launcherd"

int docker_launcher_client_process(char *imageName, int command)
{
	GError *error=NULL;
	GVariant *ret_value = NULL;
	gchar *image_name = NULL;
	gint32 read = 0;
	gint32 ret = 0;

	error = NULL;

	if(create_connection == NULL)
	{
		create_connection = g_dbus_connection_new_for_address_sync(DOCKER_LAUNCHER_PATH,
												G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
												NULL,
												NULL,
												&error);
		if(create_connection == NULL)
		{
			g_printerr("Error connection to D-bus address %s: %s\n", DOCKER_LAUNCHER_PATH, error->message);
			g_error_free(error);
			return -1;
		}
	}

	printf("[CLIENT] docker image = %s, docker command = %d\n", imageName, command);
	
	image_name = g_strdup(imageName);

	ret_value = g_dbus_connection_call_sync(create_connection, NULL, DOCKER_LAUNCHER_SERVICE,
							DOCKER_LAUNCHER_INTERFACE,METHOD_NAME,
							g_variant_new("(si)", image_name, command),
							G_VARIANT_TYPE("(i)"),
							G_DBUS_CALL_FLAGS_NONE,
							-1,
							NULL,
							&error);

	if(ret_value == NULL)
	{
		g_printerr("Error invoking %s () : %s\n", METHOD_NAME, error->message);
		g_error_free(error);
		return -1;
	}
	g_variant_get(ret_value, "(i)", ret);

	g_variant_unref(ret_value);

	return 0;
}
