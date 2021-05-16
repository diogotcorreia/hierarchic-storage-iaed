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
			parent_file->children_tree =
				insert_tree(parent_file->children_tree, current_file);
		}
		parent_file = current_file;

		name = strtok(NULL, PATH_SEPARATOR);
	}

	set_file_value(parent_file, value); /* set value of the last file in path */
}

file_t* get_file_by_path(storage_t* storage, char* path) {
	file_t* parent_file = storage->root_file;
	char* name;

	name = strtok(path, PATH_SEPARATOR);

	while (name != NULL && parent_file != NULL) {
		parent_file = get_child_by_name(parent_file, name);

		name = strtok(NULL, PATH_SEPARATOR);
	}

	return parent_file;
}

/* Files */

file_t* init_file(file_t* parent, char* name) {
	file_t* new_file = (file_t*)malloc(sizeof(file_t));

	new_file->parent = parent;
	new_file->children_by_creation = NULL;
	new_file->children_tree = NULL;
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
	return get_link_by_value(parent->children_tree, name);
}

void destroy_file(file_t* file) {
	if (file->name != NULL) free(file->name);
	if (file->value != NULL) free(file->value);
	if (file->children_tree != NULL) {
		destroy_tree(file->children_tree);
	}
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

/* AVL Tree */

link_t* init_link(file_t* file, link_t* left, link_t* right) {
	link_t* new_link = (link_t*)malloc(sizeof(link_t));

	new_link->value = file;
	new_link->left = left;
	new_link->right = right;
	new_link->height = 1;

	return new_link;
}

int height(link_t* link) {
	if (link == NULL) return 0;
	return link->height;
}

void update_height(link_t* link) {
	int height_left = height(link->left);
	int height_right = height(link->right);
	link->height =
		height_left > height_right ? height_left + 1 : height_right + 1;
}

link_t* rotL(link_t* link) {
	link_t* x = link->right;
	link->right = x->left;
	x->left = link;

	update_height(link);
	update_height(x);

	return x;
}

link_t* rotR(link_t* link) {
	link_t* x = link->left;
	link->left = x->right;
	x->right = link;

	update_height(link);
	update_height(x);

	return x;
}

link_t* rotLR(link_t* link) {
	if (link == NULL) return link;
	link->left = rotL(link->left);
	return rotR(link);
}

link_t* rotRL(link_t* link) {
	if (link == NULL) return link;
	link->right = rotR(link->right);
	return rotL(link);
}

int balance_factor(link_t* link) {
	if (link == NULL) return 0;
	return height(link->left) - height(link->right);
}

link_t* balance(link_t* link) {
	int bal_factor;
	if (link == NULL) return link;
	bal_factor = balance_factor(link);
	if (bal_factor > 1) {
		if (balance_factor(link->left) >= 0)
			link = rotR(link);
		else
			link = rotLR(link);
	} else if (bal_factor < -1) {
		if (balance_factor(link->right) <= 0)
			link = rotL(link);
		else
			link = rotRL(link);
	} else {
		update_height(link);
	}
	return link;
}

link_t* insert_tree(link_t* link, file_t* file) {
	if (link == NULL) return init_link(file, NULL, NULL);

	if (strcmp(file->name, link->value->name) < 0)
		link->left = insert_tree(link->left, file);
	else
		link->right = insert_tree(link->right, file);
	link = balance(link);
	return link;
}

file_t* get_link_by_value(link_t* link, char* value) {
	int cmp;
	if (link == NULL) return NULL;

	cmp = strcmp(value, link->value->name);
	if (cmp == 0) {
		return link->value;
	} else if (cmp < 0) {
		return get_link_by_value(link->left, value);
	} else {
		return get_link_by_value(link->right, value);
	}
}

void destroy_tree(link_t* link) {
	if (link == NULL) return;
	destroy_tree(link->left);
	destroy_tree(link->right);

	/* no need to free the value since that's handled in the linked list */
	free(link);
}
