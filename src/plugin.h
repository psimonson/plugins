/* plugin.h - simple plugin loader (header file) */


#ifndef PLUGIN_H
#define PLUGIN_H

typedef struct PLUGIN_MANAGER plugin_manager_t;
typedef struct PLUGIN_DISCOVERY_STATE plugin_discovery_state_t;

typedef int (*plugin_init_func)(plugin_manager_t* pm);
plugin_manager_t* plugin_manager_new();
void plugin_manager_free(plugin_manager_t* pm);

void discover_plugins(const char *dirname, plugin_manager_t* pm);
void cleanup_plugins(plugin_manager_t* pm);

#endif

