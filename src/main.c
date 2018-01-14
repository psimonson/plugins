#include "common/logger.h"
#include "common/prs_string.h"
#include "plugin.h"

const char key[] = "FSKDJLKSJFFfhf890w983ry38947@^#*(&^$&*";

#undef LOG_FILE
#define LOG_FILE "log.txt"

/* program to load plugins and execute them */
int main(int argc, char **argv)
{
	plugin_manager_t* pm;

	printf("TODO: Add plugin loading here.\n");
	write_log(LOG_FILE, "Plugin loading here.\n");
	pm = plugin_manager_new();
	discover_plugins("plugins", pm);
	cleanup_plugins(pm);
	plugin_manager_free(pm);
	return 0;
}

