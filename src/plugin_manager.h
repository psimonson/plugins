/* plugin_manager.h - simple plugin loader (header file) */

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

typedef char bool;
enum { FALSE, TRUE };

#define false ((char)0)
#define true ((char)1)

typedef struct PLUGIN_HOOK_LIST plugin_hook_list_t;
typedef struct PLUGIN_LIST plugin_list_t;
typedef struct PLUGIN_MANAGER {
	char *plugin_directory;
	bool autoload;
	plugin_list_t *plugin_list;
	plugin_hook_list_t *plugin_hook_list;
} plugin_manager_t;

typedef int (*plugin_init_func)(plugin_manager_t*, int, char**);

struct PLUGIN_LIST {
	char* name;
	plugin_init_func init;
	struct PLUGIN_LIST* next;
};

typedef struct PLUGIN_STATE plugin_state_t;

plugin_manager_t* plugin_manager_new();
void plugin_list_add(plugin_manager_t* pm, const char* name, plugin_init_func func);
void plugin_hook_list_add(plugin_manager_t* pm, const char* name, plugin_init_func func);
void plugin_list_free(plugin_list_t* list);
void plugin_hook_list_free(plugin_hook_list_t* list);
void plugin_manager_free(plugin_manager_t* pm);

/* plugin_manager - hook stuff below */

typedef struct command command_t;
typedef int (*command_hook)(int argc, char **argv);
struct PLUGIN_HOOK_LIST {
	command_hook hook;
	plugin_init_func init;
	int count;
	command_t* commands;
	char *name;
	plugin_hook_list_t* next;
};

struct command {
	char *cmd;
	char *help;
	int (*func)(int argc, char **argv);
};

/* command hook callback */
typedef void (*register_hook)(plugin_manager_t*, const char*,
		command_t*, int, command_hook);

/* command hook function prototypes */
void register_command_hook(plugin_manager_t*, command_t*, int, command_hook);
void init_command_hook(plugin_manager_t*, int, char**);
int apply_command_hook(plugin_manager_t*, int, char**);
void apply_command_help_hook(plugin_manager_t*);

/* load plugin for usage */
void load_plugin(plugin_manager_t*, const char*, int, char **);

#endif

