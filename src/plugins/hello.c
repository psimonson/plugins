#include "../common/logger.h"
#include "../common/prs_string.h"
#include "../plugin_manager.h"

#ifdef LOG_FILE
#undef LOG_FILE
#endif

#define LOG_FILE "log_hello.txt"

const char key[] = "DJKFOWIEJFSDFlkj7847387";

void greet(const char *name)
{
	printf("Hello, %s.\n", name);
}

int init_hello(plugin_manager_t* pm)
{
	char fullname[80];

	printf("Enter your fullname: ");
	readln(fullname, sizeof fullname);
	if (strcmp(fullname, "") == 0) {
		printf("You need to enter your name.\n");
		return -1;
	} else {
		greet(fullname);
	}
	return 0;
}

