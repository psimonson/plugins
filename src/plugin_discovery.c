#include "plugin_discovery.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>

/* --------- structures below ------------ */

typedef struct HANDLE_LIST {
	void* handle;
	struct HANDLE_LIST* next;
} plugin_list_t;

typedef struct PLUGIN_STATE {
	plugin_list_t* handle_list;
} plugin_state_t;

typedef int (*plugin_init_func)(plugin_manager_t*);

/* --------- Discover Plugins ------------ */

void get_plugin_name(char *stripped_name, int size, char *filename)
{
	char* last_slash;
	char* name_start;
	char* last_dot;
	int len;

	last_slash = strrchr(filename, '/');
	name_start = last_slash ? last_slash+1 : filename;
	last_dot = strrchr(filename, '.');

	/* only care about filenames */
	if (!last_dot || strcmp(last_dot, ".so"))
		return;

	len = (last_dot-name_start);
	memset(stripped_name, 0, size);
	if (len <= size) {
		strncpy(stripped_name, name_start, len);
		return;
	}
	printf("Name length to large to fit into buffer.\n");
}

void* plugin_load(char *filename, plugin_manager_t* pm)
{
	plugin_init_func initfunc;
	void* plugin;
	char stripped_name[256];
	char initfunc_name[512];
	int rc;

	get_plugin_name(stripped_name, sizeof stripped_name, filename);
	memset(initfunc_name, 0, sizeof initfunc_name);
	sprintf(initfunc_name, "init_%s", stripped_name);

	plugin = dlopen(filename, RTLD_NOW);
	if (plugin == NULL) {
		printf("Cannot load plugin '%s'.\n", filename);
		return NULL;
	}
	initfunc = (plugin_init_func) (intptr_t) dlsym(plugin, initfunc_name);
	if (initfunc == NULL) {
		printf("Cannot load init function.\n");
		dlclose(plugin);
		return NULL;
	}
	printf("Loaded plugin from: '%s'\n", filename);
	rc = initfunc(pm);
	if (rc < 0) {
		printf("Error occured inside plugin '%s'.\n", initfunc_name);
		dlclose(plugin);
		return NULL;
	}
	return plugin;
}

void* discover_plugins(const char *dirname, plugin_manager_t* pm)
{
	plugin_state_t* state; 
	DIR *d;
	struct dirent *dp;

	d = opendir(dirname);
	if (d == NULL) {
		printf("Unable to open path '%s'\n", dirname);
		return NULL;
	}

	state = malloc(sizeof(plugin_state_t));
	state->handle_list = NULL;

	while ((dp = readdir(d))) {
		if ((strcmp(dp->d_name, ".") != 0
		&& strcmp(dp->d_name, "..") != 0)) {
			char fullpath[256];
			void *handle;

			memset(fullpath, 0, sizeof fullpath);
			sprintf(fullpath, "%s/%s", dirname, dp->d_name);
			handle = plugin_load(fullpath, pm);
			if (handle) {
				plugin_list_t* node;
				node = malloc(sizeof(plugin_list_t));
				node->handle = handle;
				node->next = state->handle_list;
				state->handle_list = node;
			}
		}
	}
	closedir(d);
	/* return state if plugins were found */
	if (state->handle_list)
		return (void*)state;
	free(state);
	return NULL;
}


/* ------------------ Cleanup Plugins ---------------------- */


void cleanup_plugins(void* vpds)
{
	plugin_state_t* state;
	plugin_list_t* node;

	state = (plugin_state_t*)vpds;
	if (!state)
		return;
	node = state->handle_list;
	while (node) {
		plugin_list_t* next = node->next;
		dlclose(node->handle);
		free(node);
		node = next;
	}
	free(state);
}
