/* plugin.c - Plugin manager (Written in C) */

#include "plugin_manager.h"

#include <stdio.h>
#include <stdlib.h>

/* -------------------- Structures Below ------------------ */

struct PLUGIN_MANAGER {
	int active;
};

typedef int (*plugin_init_func)(plugin_manager_t*);

/* -------------------- Functions Below ------------------- */


plugin_manager_t* plugin_manager_new()
{
	plugin_manager_t* pm;
	pm = malloc(sizeof(plugin_manager_t));
	if (!pm)
		return NULL;
	pm->active = 1;
	return pm;
}

void plugin_manager_free(plugin_manager_t* pm)
{
	if (pm)
		free(pm);
}
