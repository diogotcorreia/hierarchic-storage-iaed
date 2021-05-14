/* Diogo Torres Correia, ist199211 */

#include "proj2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	/*storage global_store = {0};*/

	/* execute program until the user sends the 'quit' command */
	while (handle_command(/*&global_store*/)) {
	}

	return 0;
}

int handle_command(/*storage *global_store*/) {
	char *command = malloc(sizeof(char) * MAX_COMMAND_SIZE);
	char *command_name;
	short result = 0;

	command = fgets(command, MAX_COMMAND_SIZE, stdin);
	if (command == NULL) return 0; /* EOF or error while reading from stdin */

	command_name = strtok(command, " \t\n");

	if (strcmp(command_name, HELP_CMD) == 0) {
		handle_help_command();
		result = 1;
	}

	free(command);

	/* if command_name is quit, return the default 'result' which is 0 */
	return result;
}

void handle_help_command() {
	printf(HELP_COMMAND_FORMAT, HELP_CMD, HELP_DESC);
	printf(HELP_COMMAND_FORMAT, QUIT_CMD, QUIT_DESC);
	printf(HELP_COMMAND_FORMAT, SET_CMD, SET_DESC);
	printf(HELP_COMMAND_FORMAT, PRINT_CMD, PRINT_DESC);
	printf(HELP_COMMAND_FORMAT, FIND_CMD, FIND_DESC);
	printf(HELP_COMMAND_FORMAT, LIST_CMD, LIST_DESC);
	printf(HELP_COMMAND_FORMAT, SEARCH_CMD, SEARCH_DESC);
	printf(HELP_COMMAND_FORMAT, DELETE_CMD, DELETE_DESC);
}