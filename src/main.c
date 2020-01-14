#include "plugin_manager.h"
#include "plugin_discovery.h"

#include <stdio.h>
#include <string.h>

/* Get user input from file.
 */
int get_input(const char msg[], char s[], int size, FILE *fp)
{
	int c, i;
	printf("%s", msg);
	for(i = 0; i < size && (c = fgetc(fp)) != EOF && c != '\n'; i++)
		s[i] = c;
	s[i] = 0;
	return i;
}
/* Split string into tokens.
 */
char **split_string(char *s, int *argc)
{
#define MAXTOKENS 100
	static char *tokens[MAXTOKENS];
	char *tmp;
	int i;
	tmp = strtok(s, " \r\n");
	for(i = 0; tmp != NULL; i++) {
		tokens[i] = tmp;
		tmp = strtok(NULL, " \r\n");
	}
	tokens[i] = NULL;
	if(argc != NULL) *argc = i;
	return tokens;
#undef MAXTOKENS
}
/* Program to load plugins and execute them.
 */
int main(void)
{
	plugin_manager_t *pm;
	plugin_state_t *pdstate;
	char buf[512], **argv;
	int argc;

	pm = plugin_manager_new();
	pdstate = discover_plugins("plugins", pm);
	do {
		(void)get_input("Enter command: ", buf, sizeof(buf), stdin);
		if(strcmp(buf, "exit") == 0) break;
		argv = split_string(buf, &argc);
		if(argc == 0) continue;
		else {
			load_plugin(pm, argv[0], argc, argv);
		}
	} while(1);
	plugin_manager_free(pm);
	cleanup_plugins(pdstate);
	return 0;
}

