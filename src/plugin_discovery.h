#ifndef PLUGIN_DISCOVERY_H
#define PLUGIN_DISCOVERY_H

#include "plugin_manager.h"

void* discover_plugins(const char *dirname, plugin_manager_t* pm);
void cleanup_plugins(void*);

#endif

