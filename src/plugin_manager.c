/* plugin_manager.c - Plugin manager (Written in C) */

#include "plugin_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
/* Copy string from one location to another.
 */
int str_copy(char *s, size_t size, const char *t, size_t len)
{
	char *p = (char*)t;
	size_t i;
	for(i = 0; (i < size || --len > 0) && (*s++ = *p++) != 0; i++);
	*s = 0;
	return i;
}
/* Duplicate a string into another block of memory.
 */
char *strdup(const char *s)
{
	const int len = strlen(s);
	char *p = malloc(len*sizeof(char));
	if(!p) return NULL;
	if(str_copy(p, len, s, len) != len) { free(p); return NULL; }
	return p;
}
#endif

/* ------------------ Plugin Manager Below ---------------- */

/* Create a new plugin manager.
 */
plugin_manager_t* plugin_manager_new(void)
{
	plugin_manager_t* pm;
	pm = malloc(sizeof(plugin_manager_t));
	if (!pm) return NULL;
	pm->autoload = false;
	pm->plugin_list = NULL;
	pm->plugin_hook_list = NULL;
	return pm;
}
/* Register command hook inside program.
 */
void register_command_hook(plugin_manager_t* pm, command_t *commands,
	int cmd_count, command_hook hook)
{
	plugin_hook_list_t* node = malloc(sizeof(*node));
	if (!node || !commands) return;
	node->hook = hook;
	node->name = calloc(1, 256);
	if (!node->name) {
		free(node);
		return;
	}
	node->commands = commands;
	node->count = cmd_count;
	node->next = pm->plugin_hook_list;
	pm->plugin_hook_list = node;
}
/* Initialize command hook.
 */
void init_command_hook(plugin_manager_t* pm, int argc, char **argv)
{
	plugin_hook_list_t* plugin = pm->plugin_hook_list;
	while (plugin) {
		plugin->init(pm, argc, argv);
		plugin = plugin->next;
	}
}
/* Apply command hook (Checks for command inside plugin).
 */
int apply_command_hook(plugin_manager_t* pm, int argc, char **argv)
{
	plugin_hook_list_t* plugin = pm->plugin_hook_list;
	while (plugin) {
		int i;

		for (i = 0; i < plugin->count; i++)
			if (!strcmp(*argv, plugin->commands[i].cmd))
				return plugin->hook(argc, argv);

		plugin = plugin->next;
	}
	return -1;
}
/* Apply help hook for plugin command hook.
 */
void apply_command_help_hook(plugin_manager_t* pm)
{
	plugin_hook_list_t* command_plugin = pm->plugin_hook_list;
	while (command_plugin) {
		const command_t *commands = command_plugin->commands;
		int i;
			
		if (commands) {
			for (i=0; i<command_plugin->count; i++) {
				printf("%s - %s\r\n",
					commands[i].cmd,
					commands[i].help);
			}
			return;
		}
		command_plugin = command_plugin->next;
	}
}
/* Add a plugin to the plugin list.
 */
void plugin_list_add(plugin_manager_t* pm, const char *name,
	plugin_init_func func)
{
	plugin_list_t* node;
	node = malloc(sizeof(plugin_list_t));
	if (!node) return;
	node->name = strdup(name);
	node->init = func;
	node->next = pm->plugin_list;
	pm->plugin_list = node;
}
/* Add a plugin to the hook list.
 */
void plugin_hook_list_add(plugin_manager_t* pm, const char *name,
	plugin_init_func func)
{
	plugin_hook_list_t* node;
	node = malloc(sizeof(plugin_hook_list_t));
	if (!node) return;
	node->name = strdup(name);
	node->init = func;
	node->next = pm->plugin_hook_list;
	pm->plugin_hook_list = node;
}
/* Free plugin list.
 */
void plugin_list_free(plugin_list_t* list)
{
	plugin_list_t* node;
	if (!list) return;
	node = list;
	while (node) {
		plugin_list_t* next;
		next = node->next;
		free(node->name);
		free(node);
		node = next;
	}
}
/* Free plugin hook list.
 */
void plugin_hook_list_free(plugin_hook_list_t* list)
{
	plugin_hook_list_t* node;
	if (!list) return;
	node = list;
	while (node) {
		plugin_hook_list_t* next;
		next = node->next;
		free(node->name);
		free(node);
		node = next;
	}
}
/* Free plugin manager.
 */
void plugin_manager_free(plugin_manager_t* pm)
{
	if (!pm) return;
	plugin_hook_list_free(pm->plugin_hook_list);
	plugin_list_free(pm->plugin_list);
	free(pm);
}
/* Search for a plugin by name and then load that plugin.
 */
void load_plugin(plugin_manager_t *pm, const char *name, int argc, char **argv)
{
	plugin_list_t *node;
	for(node = pm->plugin_list; node->next != NULL; node = node->next)
		if(strcmp(node->name, name) == 0) {
			node->init(pm, argc, argv);
			return;
		}
}
