#include "../common/prs_string.h"
#include "../plugin_manager.h"

#include <strings.h>

#define DOOR_ERROR -1
#define DOOR_UNLOCKED 0
#define DOOR_LOCKED 1
#define DOOR_UNKNOWN 2

#define EXIT_PROGRAM 3

int door_lock(const char *state)
{
	if (strcasecmp(state, "") == 0)
		return DOOR_ERROR;
	else if (strcasecmp(state, "lock") == 0)
		return DOOR_LOCKED;
	else if (strcasecmp(state, "unlock") == 0)
		return DOOR_UNLOCKED;
	else if (strcasecmp(state, "exit") == 0)
		return EXIT_PROGRAM;
	return DOOR_UNKNOWN;
}

int init_program101(plugin_manager_t* pm)
{
	char input[64];
	int status;
	int done;

	done = 0;
	status = DOOR_LOCKED;
	while (!done) {
		printf("Door Status: %s\n\nlock = Lock door.\n"
			"unlock = Unlock door.\nexit = Quit module.\n"
			"Enter lock status: ",
			(status) ? "LOCKED" : "UNLOCKED");
		readln(input, sizeof input);
		switch (door_lock(input)) {
			case DOOR_ERROR:
				printf("Please enter a state for the door.\n");
				status = DOOR_ERROR;
			break;
			case DOOR_LOCKED:
				if (status == DOOR_UNLOCKED) {
					printf("Door has been locked.\n");
					status = DOOR_LOCKED;
				} else {
					printf("Door already locked.\n");
				}
			break;
			case DOOR_UNLOCKED:
				if (status == DOOR_LOCKED) {
					printf("Door has been unlocked.\n");
					status = DOOR_UNLOCKED;
				} else {
					printf("Door already unlocked.\n");
				}
			break;
			case EXIT_PROGRAM:
				printf("Quitting module... ");
				done = 1;
			break;
			default:
				printf("Unknown door status was entered.\n");
				status = DOOR_UNKNOWN;
		}
	}
	printf("module finished.\n");
	return 0;
}
