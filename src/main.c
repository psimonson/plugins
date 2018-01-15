#include "plugin_discovery.h"
#include "plugin_manager.h"

#include <stdio.h>
#include <stdlib.h>

/* program to load plugins and execute them */
int main(int argc, char **argv)
{
	plugin_manager_t* pm;
	void* pdstate;

	printf("Hello from main()\n");
	pm = plugin_manager_new();
	pdstate = discover_plugins("plugins", pm);
	plugin_manager_free(pm);
	cleanup_plugins(pdstate);
	return 0;
}

