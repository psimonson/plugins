#include "common/logger.h"
#include "common/prs_string.h"

const char key[] = "FSKDJLKSJFFfhf890w983ry38947@^#*(&^$&*";

#undef LOG_FILE
#define LOG_FILE "log.txt"

/* program to load plugins and execute them */
int main(int argc, char **argv)
{
	char buf[80];

	printf("Enter plugin name: ");
	readln(buf, sizeof buf);

	if (strcmp(buf, "") == 0) {
		printf("You need to enter a plugin name.\n");
		write_log(LOG_FILE, "You need to enter a plugin name.\n");
	} else {
		printf("TODO: Add plugin loading here.\n");
		write_log(LOG_FILE, "TODO: Add plugin loading here.\n");
	}

	return 0;
}

