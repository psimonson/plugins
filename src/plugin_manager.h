/* plugin.h - simple plugin loader (header file) */

#ifndef PLUGIN_H
#define PLUGIN_H

typedef struct PLUGIN_MANAGER plugin_manager_t;

plugin_manager_t* plugin_manager_new();
void plugin_manager_free(plugin_manager_t* pm);

#endif

