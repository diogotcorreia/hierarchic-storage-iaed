/* Diogo Torres Correia, ist199211 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proj2.h"
#include "structures.h"

/**
 * Insert path and value into storage, creating all parent files (directories)
 * in the path.
 * The path is split by '/', and each component is a different file (or
 * directory).
 */
void add_path_recursively(storage_t* storage, char* path, char* value) {
	file_t* parent_file = storage->root_file;
	char* name;

	/* split the file path by slashes */
	name = strtok(path, PATH_SEPARATOR);

	while (name != NULL) {
		file_t* current_file = get_child_by_name(parent_file, name);

		if (current_file == NULL) {
			/* create each file in the path if it does not exist */
			list_t* parent_children;
			current_file = init_file(parent_file, name);
			parent_children = get_file_children_by_creation(parent_file);
			insert_list(parent_children, current_file);
			parent_file->children_tree = insert_tree(
				parent_file->children_tree, current_file, get_file_name);
		}
		parent_file = current_file;

		name = strtok(NULL, PATH_SEPARATOR);
	}

	/* set value of the last file in path */
	set_file_value(storage, parent_file, value);
}

/**
 * Gets a a file by its full path.
 * Returns NULL if the file does not exist.
 */
file_t* get_file_by_path(storage_t* storage, char* path) {
	file_t* parent_file = storage->root_file;
	char* name;

	/* split the file path by slashes */
	name = strtok(path, PATH_SEPARATOR);

	while (name != NULL && parent_file != NULL) {
		parent_file = get_child_by_name(parent_file, name);

		name = strtok(NULL, PATH_SEPARATOR);
	}

	return parent_file;
}

/**
 * Initializes a file struct. Creates a new file with the given name
 * and parent, and without any value (its set as NULL).
 * The file name is copied into the file.
 * Returns the newly allocated file.
 */
file_t* init_file(file_t* parent, char* name) {
	file_t* new_file = (file_t*)safe_malloc(sizeof(file_t));

	new_file->parent = parent;
	new_file->children_by_creation = NULL;
	new_file->children_tree = NULL;
	new_file->value = NULL;

	/* the file name can be NULL if it is the root file */
	if (name != NULL) {
		new_file->name = (char*)safe_malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(new_file->name, name);
	} else {
		new_file->name = NULL;
	}

	return new_file;
}

/**
 * Sets or updates the value of a given file.
 * The value is copied into the file.
 * The global hashtable is updated to reflect the value change.
 */
void set_file_value(storage_t* storage, file_t* file, char* value) {
	if (file->value != NULL) {
		/* discard the old stored value */
		delete_hashtable(storage->search_table, file, get_file_value);
		free(file->value);
	}

	file->value = (char*)safe_malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(file->value, value);

	storage->search_table =
		insert_hashtable(storage->search_table, file, get_file_value);
}

/**
 * Get a double linked list of file's children, sorted by when the child file
 * was created. If the parent has no children, returns an empty list.
 * Returns the list of children.
 */
list_t* get_file_children_by_creation(file_t* parent) {
	if (parent->children_by_creation == NULL) {
		parent->children_by_creation = init_list();
	}
	return parent->children_by_creation;
}

/**
 * Get the direct child of a file by its name.
 * Returns the file with that name.
 * If the file does not exist, returns NULL.
 */
file_t* get_child_by_name(file_t* parent, char* name) {
	return get_link_by_value(parent->children_tree, name, get_file_name);
}

/**
 * Removes a file and its children from storage, including from the global
 * hashtable. Additionally, frees all the memory associated with them.
 * Does not remove the file reference from its parent.
 */
void destroy_file(void* storage, void* file) {
	file_t* f = (file_t*)file;
	if (f->value != NULL)
		delete_hashtable(((storage_t*)storage)->search_table, file,
		                 get_file_value);

	if (f->name != NULL) free(f->name);
	if (f->value != NULL) free(f->value);

	/* destroy all child files */

	if (f->children_by_creation != NULL) {
		traverse_list(f->children_by_creation, storage, destroy_file);
		destroy_list(f->children_by_creation);
	}
	if (f->children_tree != NULL) {
		destroy_tree(f->children_tree);
	}

	free(f);
}

/**
 * Removes a file from its parent (if exists) and then destroys the file
 * using destroy_file.
 */
void delete_file(storage_t* storage, file_t* file) {
	int has_parent = file->parent != NULL;
	if (has_parent) {
		delete_node(file->parent->children_by_creation, file);
		file->parent->children_tree =
			delete_link(file->parent->children_tree, file->name, get_file_name);
	}
	destroy_file(storage, file);
	if (!has_parent) {
		/* since the root cannot be deleted, recreate it */
		storage->root_file = init_file(NULL, NULL);
	}
}

/**
 * Prints the path and value of each file and its children, recursively.
 * It only prints information about files that have a value.
 * If a file does not have a value, its path is NOT printed to stdout.
 */
void print_files_recursively(void* data, void* value) {
	file_t* file = (file_t*)value;
	if (file->value != NULL) {
		print_file_path(file);
		printf(PATH_VALUE_FORMAT, file->value);
	}
	if (file->children_by_creation != NULL) {
		traverse_list(file->children_by_creation, data,
		              print_files_recursively);
	}
}

/**
 * Prints the full path of a file to stdout.
 * Since a file only knows about its parent, and not the full parent list, the
 * full path must be recreated using recursion.
 */
void print_file_path(file_t* file) {
	if (file->parent != NULL) print_file_path(file->parent);
	if (file->name != NULL) printf(PATH_PRINT_FORMAT, file->name);
}

/**
 * Prints a "directory", that is, a set of files that share the same parent,
 * in alphabetical order by traversing a binary tree.
 */
void print_file_tree(link_t* link) {
	traverse_tree(link, print_file_name);
}

/* Auxiliaries */

/**
 * Returns the file name of a given file.
 * If the file is the root file, returns NULL.
 */
char* get_file_name(void* file) {
	return ((file_t*)file)->name;
}

/**
 * Returns the value of a given file, which might be NULL.
 */
char* get_file_value(void* file) {
	return ((file_t*)file)->value;
}

/**
 * Given a file, prints that file's name to stdout.
 */
void print_file_name(void* file) {
	printf(LIST_FORMAT, ((file_t*)file)->name);
}
