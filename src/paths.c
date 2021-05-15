/* Diogo Torres Correia, ist199211 */

#include <stdlib.h>
#include <string.h>

#include "proj2.h"

void add_path_recursively(storage_t* storage, char* path, char* value) {
	file_t* parent_file = storage->root_file;
	char* name;

	name = strtok(path, PATH_SEPARATOR);

	while (name != NULL) {
		file_t* current_file = get_child_by_name(parent_file, name);

		if (current_file == NULL) {
			list_t* parent_children;
			current_file = init_file(parent_file, name);
			parent_children = get_file_children_by_creation(parent_file);
			insert_list(parent_children, current_file);
		}
		/* TODO insert into binary tree */
		parent_file = current_file;

		name = strtok(NULL, PATH_SEPARATOR);
	}

	set_file_value(parent_file, value); /* set value of the last file in path */
}

/* Files */

file_t* init_file(file_t* parent, char* name) {
	file_t* new_file = (file_t*)malloc(sizeof(file_t));

	new_file->parent = parent;
	new_file->children_by_creation = NULL;
	new_file->value = NULL;

	if (name != NULL) {
		new_file->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(new_file->name, name);
	} else {
		new_file->name = NULL;
	}

	return new_file;
}

void set_file_value(file_t* file, char* value) {
	file->value = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(file->value, value);
}

list_t* get_file_children_by_creation(file_t* parent) {
	if (parent->children_by_creation == NULL) {
		parent->children_by_creation = init_list();
	}
	return parent->children_by_creation;
}

file_t* get_child_by_name(file_t* parent, char* name) {
	/* TODO in the future, use the binary tree for better performance */
	file_t* result = NULL;

	if (parent->children_by_creation != NULL) {
		node_t* aux = parent->children_by_creation->first;

		while (aux != NULL && result == NULL) {
			if (strcmp(aux->value->name, name) == 0) result = aux->value;

			aux = aux->next;
		}
	}

	return result;
}

void destroy_file(file_t* file) {
	if (file->name != NULL) free(file->name);
	if (file->value != NULL) free(file->value);
	if (file->children_by_creation != NULL) {
		destroy_list(file->children_by_creation);
	}

	free(file);
}

/* Linked List */

list_t* init_list() {
	list_t* new_list = malloc(sizeof(list_t));
	new_list->first = NULL;
	new_list->last = NULL;
	return new_list;
}

void insert_list(list_t* list, file_t* item) {
	node_t* node = (node_t*)malloc(sizeof(node_t));

	node->value = item;

	node->next = NULL;
	node->prev = list->last;
	if (list->first == NULL) {
		list->first = node;
	} else {
		list->last->next = node;
	}
	list->last = node;
}

void destroy_list(list_t* list) {
	node_t* next = list->first;

	while (next != NULL) {
		node_t* aux = next;
		next = aux->next;

		destroy_file(aux->value);
		free(aux);
	}

	free(list);
}