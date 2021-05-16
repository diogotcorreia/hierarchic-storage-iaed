/* Diogo Torres Correia, ist199211 */

#include "proj2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int handle_command(storage_t *storage, char *command) {
	char *command_name;
	short result = 0;

	command = fgets(command, MAX_COMMAND_SIZE, stdin);
	if (command == NULL) return 0; /* EOF or error while reading from stdin */

	command_name = strtok(command, WHITESPACE);

	if (command_name != NULL) {
		if (strcmp(command_name, HELP_CMD) == 0) {
			handle_help_command();
			result = 1;
		} else if (strcmp(command_name, SET_CMD) == 0) {
			handle_set_command(storage, command + strlen(command_name) + 1);
			result = 1;
		} else if (strcmp(command_name, PRINT_CMD) == 0) {
			handle_print_command(storage);
			result = 1;
		} else if (strcmp(command_name, FIND_CMD) == 0) {
			handle_find_command(storage, command + strlen(command_name) + 1);
			result = 1;
		} else if (strcmp(command_name, LIST_CMD) == 0) {
			handle_list_command(storage, command + strlen(command_name) + 1);
			result = 1;
		} else if (strcmp(command_name, DELETE_CMD) == 0) {
			handle_delete_command(storage, command + strlen(command_name) + 1);
			result = 1;
		}
	}

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

void handle_set_command(storage_t *storage, char *arguments) {
	char *path;
	char *value;

	path = strtok(arguments, WHITESPACE);
	arguments += strlen(path) + 1; /* skip to next command argument */

	value = trim_whitespace(arguments);

	add_path_recursively(storage, path, value);
}

void handle_print_command(storage_t *storage) {
	print_file_recursively(storage->root_file);
}

void print_file_recursively(file_t *file) {
	if (file->value != NULL) {
		print_file_path(file);
		printf(PATH_VALUE_FORMAT, file->value);
	}
	if (file->children_by_creation != NULL) {
		node_t *aux = file->children_by_creation->first;
		while (aux != NULL) {
			print_file_recursively(aux->value);
			aux = aux->next;
		}
	}
}

void print_file_path(file_t *file) {
	if (file->parent != NULL) print_file_path(file->parent);
	if (file->name != NULL) printf(PATH_PRINT_FORMAT, file->name);
}

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

void print_file_tree(link_t *link) {
	if (link == NULL) return;

	print_file_tree(link->left);
	printf(LIST_FORMAT, link->value->name);
	print_file_tree(link->right);
}

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
