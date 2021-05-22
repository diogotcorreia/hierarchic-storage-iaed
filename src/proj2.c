/* Diogo Torres Correia, ist199211 */

#include "proj2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * The main function of the program.
 * Creates the global storage struct, root files and hashtable.
 * Repeatedly waits for a new command.
 * Ends the program by freeing all the used memory.
 */
int main() {
	storage_t *storage = (storage_t *)malloc(sizeof(storage_t));
	char *command = malloc(sizeof(char) * MAX_COMMAND_SIZE);

	storage->root_file = init_file(NULL, NULL);
	storage->search_table = init_hashtable(HASHTABLE_START_SIZE);

	/* execute program until the user sends the "quit" command */
	while (handle_command(storage, command)) {
	}

	free(command);
	destroy_file(storage, storage->root_file);
	destroy_hashtable(storage->search_table);
	free(storage);

	return 0;
}

/**
 * Handles command input.
 * A string is passed as an argument as a buffer to store the command
 * from stdin, which should be reused across commands.
 * If the program should continue after the command, returns 1.
 * Otherwise returns 0.
 */
short handle_command(storage_t *storage, char *command) {
	char *command_name;
	short result = 1; /* 1 continues the program, 0 quits */

	command = fgets(command, MAX_COMMAND_SIZE, stdin);
	if (command == NULL) return 0; /* EOF or error while reading from stdin */

	command_name = strtok(command, WHITESPACE);

	if (command_name != NULL) {
		if (strcmp(command_name, HELP_CMD) == 0) {
			handle_help_command();
		} else if (strcmp(command_name, SET_CMD) == 0) {
			handle_set_command(storage, command + strlen(command_name) + 1);
		} else if (strcmp(command_name, PRINT_CMD) == 0) {
			handle_print_command(storage);
		} else if (strcmp(command_name, FIND_CMD) == 0) {
			handle_find_command(storage, command + strlen(command_name) + 1);
		} else if (strcmp(command_name, LIST_CMD) == 0) {
			handle_list_command(storage, command + strlen(command_name) + 1);
		} else if (strcmp(command_name, SEARCH_CMD) == 0) {
			handle_search_command(storage, command + strlen(command_name) + 1);
		} else if (strcmp(command_name, DELETE_CMD) == 0) {
			handle_delete_command(storage, command + strlen(command_name) + 1);
		} else if (strcmp(command_name, QUIT_CMD) == 0) {
			result = 0;
		}
	}

	return result;
}

/**
 * Handles the 'help' command, printing each command and its description.
 */
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

/**
 * Handles the 'set' command.
 * Receives the arguments of the command, and extracts the path and value,
 * passing them to the add path function.
 */
void handle_set_command(storage_t *storage, char *arguments) {
	char *path;
	char *value;

	path = strtok(arguments, WHITESPACE);
	arguments += strlen(path) + 1; /* skip to next command argument */

	value = trim_whitespace(arguments);

	add_path_recursively(storage, path, value);
}

/**
 * Handles the 'print' command.
 * Prints the path and value of every known file.
 */
void handle_print_command(storage_t *storage) {
	print_files_recursively(NULL, storage->root_file);
}

/**
 * Handles the 'find' command.
 * Receives a path to a files and prints its value.
 * Can also print file not found or file empty errors.
 */
void handle_find_command(storage_t *storage, char *arguments) {
	file_t *file;

	arguments = trim_whitespace(arguments);

	file = get_file_by_path(storage, arguments);
	if (file == NULL) {
		printf(FIND_ERR_FILE_NOT_FOUND);
	} else if (file->value == NULL) {
		printf(FIND_ERR_FILE_EMPTY);
	} else {
		printf(FIND_FORMAT, file->value);
	}
}

/**
 * Handles the 'list' command.
 * Receives a path and prints the name of all children files, alphabetically.
 * Can also print file not found error.
 */
void handle_list_command(storage_t *storage, char *arguments) {
	file_t *file;

	arguments = trim_whitespace(arguments);

	file = get_file_by_path(storage, arguments);
	if (file == NULL) {
		printf(LIST_ERR_PATH_NOT_FOUND);
	} else {
		print_file_tree(file->children_tree);
	}
}

/**
 * Handles the 'search' command.
 * Receives a value and prints the path of the file that has that same value.
 * Can also print file not found error when no file with that value
 * does not exist.
 */
void handle_search_command(storage_t *storage, char *arguments) {
	file_t *file;

	arguments = trim_whitespace(arguments);

	file = search_hashtable(storage->search_table, arguments, get_file_value);
	if (file == NULL) {
		printf(SEARCH_ERR_PATH_NOT_FOUND);
	} else {
		print_file_path(file);
		printf("\n");
	}
}

/**
 * Handles the 'delete' command.
 * Receives a path and deletes the associated file and all its children.
 * Can also print file not found error.
 */
void handle_delete_command(storage_t *storage, char *arguments) {
	file_t *file;

	arguments = trim_whitespace(arguments);

	file = get_file_by_path(storage, arguments);
	if (file == NULL) {
		printf(DELETE_ERR_PATH_NOT_FOUND);
	} else {
		delete_file(storage, file);
	}
}

/*********/
/* Utils */
/*********/

/**
 * Checks if a character is a space, tab or newline.
 * Returns 1 if so, otherwise returns 0.
 */
short is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

/**
 * Removes whitespace (spaces, tabs or newlines) from the beginning and end
 * of the given string by modifying the string.
 * Returns a pointer to the first non-whitespace character, and places a
 * '\0' after the last non-whitespace character.
 */
char *trim_whitespace(char *str) {
	int i = 0;
	char *last_non_whitespace;
	while (is_whitespace(*str)) ++str;

	for (last_non_whitespace = str; str[i] != '\0'; ++i) {
		if (!is_whitespace(str[i])) {
			last_non_whitespace = str + i;
		}
	}

	*(last_non_whitespace + 1) = '\0';

	return str;
}
