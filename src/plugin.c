/* plugin.c - Plugin manager (Written in C) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>

#include "plugin.h"

typedef struct PLUGIN_HANDLE_LIST {
	void* handle;
	struct PLUGIN_HANDLE_LIST* next;
} plugin_handle_list_t;

typedef struct PLUGIN_DISCOVERY_STATE {
	plugin_handle_list_t* handle_list;
} plugin_discovery_state_t;

struct PLUGIN_MANAGER {
	plugin_discovery_state_t* discovery;
	int active;
};

plugin_manager_t* plugin_manager_new()
{
	plugin_manager_t* pm;
	pm = malloc(sizeof(*pm));
	if (pm) {
		pm->active = 1;
		return pm;
	}
	return NULL;
}

void plugin_manager_free(plugin_manager_t* pm)
{
	free(pm);
}

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
		printf("Cannot load plugin.\n");
		cleanup_plugins(pm);
		plugin_manager_free(pm);
		return NULL;
	}
	initfunc = (plugin_init_func)dlsym(plugin, initfunc_name);
	if (initfunc == NULL) {
		printf("Cannot load init function.\n");
		cleanup_plugins(pm);
		plugin_manager_free(pm);
		return NULL;
	}
	rc = initfunc(pm);
	if (rc < 0) {
		printf("Error occured inside plugin '%s'.\n", initfunc_name);
		cleanup_plugins(pm);
		plugin_manager_free(pm);
		return NULL;
	}
	return plugin;
}

void discover_plugins(const char *dirname, plugin_manager_t* pm)
{
	char fullpath[256];
	plugin_discovery_state_t* discovery; 
	DIR *d;

	discovery = malloc(sizeof(plugin_discovery_state_t));
	if (discovery == NULL)
		return;
	d = opendir(dirname);
	while (d != NULL) {
		struct dirent *dp;
		dp = readdir(d);
		while (dp != NULL) {
			if (strcmp(dp->d_name, ".") != 0
			|| strcmp(dp->d_name, "..") != 0) {
				void *node;

				memset(fullpath, 0, sizeof fullpath);
				sprintf(fullpath, "./%s/%s", dirname, dp->d_name);
				node = plugin_load(fullpath, pm);
				if (node) {
					plugin_handle_list_t* list;
					list = malloc(sizeof *list);
					list->handle = node;
					list->next = NULL;
					discovery->handle_list->handle = node;
					discovery->handle_list->next = list;
				}
			}
		}
		closedir(d);
	}
	pm->discovery = discovery;
}

void cleanup_plugins(plugin_manager_t* pm)
{
	plugin_handle_list_t* list;
	plugin_discovery_state_t* state;

	state = (plugin_discovery_state_t*)pm->discovery;
	list = (plugin_handle_list_t*)pm->discovery->handle_list;

	if (list) {
		plugin_handle_list_t* tmp;
		tmp = list;
		while (tmp->next != NULL) {
			if (tmp->handle) {
				if (dlclose(tmp->handle) < 0) {
					printf("Error: unloading DSO plugin.\n");
				} else {
					plugin_handle_list_t* next;
					next = tmp->next;
					free(tmp);
					tmp = next;
				}
			} else {
				tmp = tmp->next;
			}
		}
	}
	free(list);
	free(state);
}

