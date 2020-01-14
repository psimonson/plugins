#include "plugin_discovery.h"
#include "plugin_manager.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <dlfcn.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>

/* --------- structures below ------------ */

typedef struct HANDLE_LIST {
	void* handle;
	struct HANDLE_LIST* next;
} handle_list_t;

struct PLUGIN_STATE {
	handle_list_t* handle_list;
};

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

#ifdef _WIN32
	/* only care about filenames */
	if (!last_dot || strcmp(last_dot, ".dll"))
		return;
#else
	/* only care about filenames */
	if (!last_dot || strcmp(last_dot, ".so"))
		return;
#endif

	len = (last_dot-name_start);
	memset(stripped_name, 0, size);
	if (len <= size) {
		strncpy(stripped_name, name_start, len);
		return;
	}
	printf("Name length to large to fit into buffer.\n");
}

void* plugin_load(char* filename, plugin_manager_t* pm)
{
	plugin_init_func initfunc;
	void* plugin;
	char stripped_name[260];
	char initfunc_name[265];

	get_plugin_name(stripped_name, sizeof stripped_name, filename);
	memset(initfunc_name, 0, sizeof initfunc_name);
	sprintf(initfunc_name, "init_%s", stripped_name);

#ifdef _WIN32
	plugin = LoadLibrary(filename);
	if (plugin == NULL) {
		printf("Cannot load plugin '%s'.\n", filename);
		return NULL;
	}
	initfunc = (plugin_init_func) (intptr_t)
			GetProcAddress(plugin, "command_hook_init");
	if (initfunc == NULL) {
		initfunc = (plugin_init_func) (intptr_t)
				GetProcAddress(plugin, initfunc_name);
		if (initfunc == NULL) {
			printf("Cannot load init function.\n");
			FreeLibrary(plugin);
			return NULL;
		} else
			plugin_list_add(pm, stripped_name, initfunc);
	} else {
		plugin_hook_list_add(pm, stripped_name, initfunc);
	}
#else
	plugin = dlopen(filename, RTLD_NOW);
	if (plugin == NULL) {
		printf("Cannot load plugin '%s'.\n", filename);
		return NULL;
	}
	initfunc = (plugin_init_func)(intptr_t)dlsym(plugin, "command_hook_init");
	if (initfunc == NULL) {
		initfunc = (plugin_init_func)(intptr_t)dlsym(plugin, initfunc_name);
		if (initfunc == NULL) {
			printf("Cannot load init function.\n");
			dlclose(plugin);
			return NULL;
		} else
			plugin_list_add(pm, stripped_name, initfunc);
	} else {
		plugin_hook_list_add(pm, stripped_name, initfunc);
	}
#endif
	printf("Loaded plugin from: '%s'\n", filename);
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
			char fullpath[512];
			void *handle;

			memset(fullpath, 0, sizeof fullpath);
			sprintf(fullpath, "%s/%s", dirname, dp->d_name);
			handle = plugin_load(fullpath, pm);
			if (handle) {
				handle_list_t* node;
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

/* plugin_reload:  reload all plugins */
/*
void plugins_reload()
{
	extern plugin_state_t* pdstate;
	extern plugin_manager_t* pm;

	plugin_manager_free(pm);
	cleanup_plugins(pdstate);
	pm = plugin_manager_new();
}
*/

void cleanup_plugins(void* vpds)
{
	plugin_state_t* state;
	handle_list_t* node;

	state = (plugin_state_t*)vpds;
	if (!state)
		return;
	node = state->handle_list;
	while (node) {
		handle_list_t* next = node->next;
#ifdef _WIN32
		FreeLibrary(node->handle);
#else
		dlclose(node->handle);
#endif
		free(node);
		node = next;
	}
	free(state);
}
