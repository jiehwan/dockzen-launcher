#ifndef __DOCKER_LAUNCHER_TEMP_H__
#define __DOCKER_LAUNCHER_TEMP_H__

#include <wifi.h>

static void WiFiScanCallback(wifi_error_e err, void* data);
static void WiFiActivateCallback(wifi_error_e result, void* user_data);
static void wifi_test(void);
void check_network_connection(void);

#define WLAN_EXEC "wlan.sh start"

#endif /* __DOCKER_LAUNCHER_TEMP_H__ */

