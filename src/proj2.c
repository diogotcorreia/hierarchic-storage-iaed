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

	/* execute program until the user sends the 'quit' command */
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
int handle_command(storage_t *storage, char *command) {
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
	print_file_recursively(storage->root_file);
}

/**
 * Prints the path and value of each file and its children, recursively.
 * It only prints information about files that have a value.
 * If a file does not have a value, its path is NOT printed to stdout.
 */
void print_file_recursively(file_t *file) {
	if (file->value != NULL) {
		print_file_path(file);
		printf(PATH_VALUE_FORMAT, file->value);
	}
	if (file->children_by_creation != NULL) {
		/* TODO use a new traverse_list function */
		node_t *aux = file->children_by_creation->first;
		while (aux != NULL) {
			print_file_recursively(aux->value);
			aux = aux->next;
		}
	}
}

/**
 * Prints the full path of a file to stdout.
 * Since a file only knows about its parent, and not the full parent list, the
 * full path must be recreated using recursion.
 */
void print_file_path(file_t *file) {
	if (file->parent != NULL) print_file_path(file->parent);
	if (file->name != NULL) printf(PATH_PRINT_FORMAT, file->name);
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
 * Prints a "directory", that is, a set of files that share the same parent,
 * in alphabetical order by traversing a binary tree.
 */
void print_file_tree(link_t *link) {
	/* TODO add traverse tree function */
	if (link == NULL) return;

	print_file_tree(link->left);
	printf(LIST_FORMAT, link->value->name);
	print_file_tree(link->right);
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

	file = search_hashtable(storage->search_table, arguments);
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
