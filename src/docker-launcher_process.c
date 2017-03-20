#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "docker-launcher_dbus.h"
#include "docker-launcher_service.h"


typedef enum
{
	COMMAND_CREATE = 1,
	COMMAND_UPDATE,
	COMMAND_NONE
} DOCKER_LAUNCHER_COMMAND;

static GDBusNodeInfo *introspection_data = NULL;

static int docker_launcher_loop(char *image_name , int cmd)
{

	switch(cmd)
	{
	case COMMAND_CREATE:
		printf("create!!!\n");
		createService(image_name);
		break;
	case COMMAND_UPDATE:
		printf("update!!!\n");
		updateService(image_name);
		break;
	}
	
	return 0;
}

static void handle_method_call(GDBusConnection *connection,
							const gchar	*sender,
							const gchar *object_path,
							const gchar *interface_name,
							const gchar *method_name,
							GVariant	*parameters,
							GDBusMethodInvocation 	*invocation,
							gpointer	user_data)
{
	gchar *image_name;
	gint32 command = 0;
	gint32 ret = 0;

	if(parameters == NULL)
	{
		g_print("client : parameters is NULL\n");
		return ;
	}
	if(invocation == NULL)
	{
		g_print("client: invocation is NULL\n");
		return ;
	}
	if(method_name == NULL)
	{
		g_print("client : method_name is NULL\n");
		return ;
	}

	{
		g_variant_get(parameters, "(si)", &image_name, &command);

		g_print("[SERVER] image= %s, cmd = %d\n", image_name, command);

		docker_launcher_loop(image_name, command);
		

		g_dbus_method_invocation_return_value(invocation, g_variant_new("(i)", 1));
	}
}

static const GDBusInterfaceVTable interface_vtable=
{
	handle_method_call,
	NULL,
	NULL,
};

guint registration_id = 0;


static gboolean on_new_connection(GDBusServer *server, GDBusConnection *connection, gpointer user_data)
{
	GCredentials *credentials;
	gchar *s;

	g_object_ref(connection);
	registration_id = g_dbus_connection_register_object(connection, DOCKER_LAUNCHER_SERVICE, introspection_data->interfaces[0], &interface_vtable, NULL, NULL, NULL);

	return TRUE;
}

int docker_launcher_process(void)
{
	GDBusServer *server;
	gchar *guid;
	GMainLoop *loop;
	GDBusServerFlags server_flags;
	GError *error;
	gint ret = 1;

	g_type_init();
	introspection_data = g_dbus_node_info_new_for_xml(docker_launcher_data_xml, NULL);
	g_assert(introspection_data != NULL);

	guid = g_dbus_generate_guid();
	server_flags = G_DBUS_SERVER_FLAGS_NONE;

	error=NULL;

	printf("\n________Daemon Start!!!________\n");

	server = g_dbus_server_new_sync(DOCKER_LAUNCHER_PATH, server_flags, guid, NULL, NULL, &error);
	if(server == NULL)
	{
		g_printerr("Error creating server at address %s: %s\n", DOCKER_LAUNCHER_PATH, error->message);
		g_error_free(error);
		return ret;
	}
	g_dbus_server_start(server);
	if(guid !=NULL)
		g_free(guid);

	g_signal_connect(server, "new-connection", G_CALLBACK(on_new_connection), NULL);
	loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

	if(server != NULL)
		g_object_unref(server);
	if(loop != NULL)
		g_main_loop_unref(loop);
	
	return 0;
}

