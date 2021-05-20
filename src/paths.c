/* Diogo Torres Correia, ist199211 */

#include <stdlib.h>
#include <string.h>

#include "proj2.h"

/**
 * Insert path and value into storage, creating all parent files (directories)
 * in the path.
 * The path is split by '/', and each component is a different file (or
 * directory).
 */
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

	name = strtok(path, PATH_SEPARATOR);

	while (name != NULL && parent_file != NULL) {
		parent_file = get_child_by_name(parent_file, name);

		name = strtok(NULL, PATH_SEPARATOR);
	}

	return parent_file;
}

/* Files */

/**
 * Initializes a file struct. Creates a new file with the given name
 * and parent, and without any value (its set as NULL).
 * The file name is copied into the file.
 * Returns the newly allocated file.
 */
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

/**
 * Sets or updates the value of a given file.
 * The value is copied into the file.
 * The global hashtable is updated to reflect the value change.
 */
void set_file_value(storage_t* storage, file_t* file, char* value) {
	if (file->value != NULL) {
		delete_hashtable(storage->search_table, file);
		free(file->value);
	}

	file->value = malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(file->value, value);

	storage->search_table = insert_hashtable(storage->search_table, file);
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
	return get_link_by_value(parent->children_tree, name);
}

/**
 * Removes a file and its children from storage, including from the global
 * hashtable. Additionally, frees all the memory associated with them.
 * Does not remove the file reference from its parent.
 */
void destroy_file(storage_t* storage, file_t* file) {
	if (file->value != NULL) delete_hashtable(storage->search_table, file);

	if (file->name != NULL) free(file->name);
	if (file->value != NULL) free(file->value);
	if (file->children_tree != NULL) {
		destroy_tree(file->children_tree);
	}
	if (file->children_by_creation != NULL) {
		destroy_list(storage, file->children_by_creation);
	}

	free(file);
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
			delete_link(file->parent->children_tree, file->name);
	}
	destroy_file(storage, file);
	if (!has_parent) {
		/* since the root cannot be deleted, recreate it */
		storage->root_file = init_file(NULL, NULL);
	}
}

/* Linked List */

/**
 * Create a new empty double linked list.
 * Returns the newly created list.
 */
list_t* init_list() {
	list_t* new_list = malloc(sizeof(list_t));
	new_list->first = NULL;
	new_list->last = NULL;
	return new_list;
}

/**
 * Inserts a given (already allocated) file into the given double linked list,
 * as the last element.
 */
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

/**
 * Removes all items (files) from the list, frees the list nodes and the
 * associated files.
 */
void destroy_list(storage_t* storage, list_t* list) {
	node_t* next = list->first;

	while (next != NULL) {
		node_t* aux = next;
		next = aux->next;

		destroy_file(storage, aux->value);
		free(aux);
	}

	free(list);
}

/**
 * Deletes a single node from the list that has the given value (file), freeing
 * itself. The file (value) is not freed.
 */
void delete_node(list_t* list, file_t* value) {
	node_t* current_node = list->first;

	while (current_node != NULL && current_node->value != value)
		current_node = current_node->next;

	if (current_node != NULL) {
		if (current_node->prev != NULL)
			current_node->prev->next = current_node->next;
		else
			list->first = current_node->next;
		if (current_node->next != NULL)
			current_node->next->prev = current_node->prev;
		else
			list->last = current_node->prev;
		free(current_node);
	}
}

/* AVL Tree */

/**
 * Creates a new AVL Tree item with the given value (file) and left/right
 * children.
 * Returns the newly created link.
 */
link_t* init_link(file_t* file, link_t* left, link_t* right) {
	link_t* new_link = (link_t*)malloc(sizeof(link_t));

	new_link->value = file;
	new_link->left = left;
	new_link->right = right;
	new_link->height = 1;

	return new_link;
}

/**
 * Gets the height of a tree link.
 * If the link is NULL, the height is 0.
 * Returns the height of the link.
 */
int height(link_t* link) {
	if (link == NULL) return 0;
	return link->height;
}

/**
 * Recalculates the height of the given link based on the height of its
 * children.
 */
void update_height(link_t* link) {
	int height_left = height(link->left);
	int height_right = height(link->right);
	link->height =
		height_left > height_right ? height_left + 1 : height_right + 1;
}

/**
 * Executes a rotation left on the given link.
 * Returns the link that is now in the position of the given link.
 */
link_t* rotL(link_t* link) {
	link_t* x = link->right;
	link->right = x->left;
	x->left = link;

	update_height(link);
	update_height(x);

	return x;
}

/**
 * Executes a rotation right on the given link.
 * Returns the link that is now in the position of the given link.
 */
link_t* rotR(link_t* link) {
	link_t* x = link->left;
	link->left = x->right;
	x->right = link;

	update_height(link);
	update_height(x);

	return x;
}

/**
 * Executes a rotation left followed by a rotation right on the given link.
 * Returns the link that is now in the position of the given link.
 */
link_t* rotLR(link_t* link) {
	if (link == NULL) return link;
	link->left = rotL(link->left);
	return rotR(link);
}

/**
 * Executes a rotation right followed by a rotation left on the given link.
 * Returns the link that is now in the position of the given link.
 */
link_t* rotRL(link_t* link) {
	if (link == NULL) return link;
	link->right = rotR(link->right);
	return rotL(link);
}

/**
 * Gets the balance factor of the given link, which is the difference
 * between the height of the left and right links.
 * If link is NULL, returns 0.
 */
int balance_factor(link_t* link) {
	if (link == NULL) return 0;
	return height(link->left) - height(link->right);
}

/**
 * Balance a tree link by executing the necessary rotations.
 * Returns the link that is now in the position of the given link.
 */
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

/**
 * Insert a new item (file) under the given link.
 * Automatically balances the resulting AVL tree.
 */
link_t* insert_tree(link_t* link, file_t* file) {
	if (link == NULL) return init_link(file, NULL, NULL);

	if (strcmp(file->name, link->value->name) < 0)
		link->left = insert_tree(link->left, file);
	else
		link->right = insert_tree(link->right, file);
	link = balance(link);
	return link;
}

/**
 * Get the file that has the given name (value) from the given tree (link).
 * Returns the found file. If the file does not exist, returns NULL.
 */
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

/**
 * Destroys the entire tree given by the root link.
 * Frees each link, but not their value.
 */
void destroy_tree(link_t* link) {
	if (link == NULL) return;
	destroy_tree(link->left);
	destroy_tree(link->right);

	/* no need to free the value since that's handled in the linked list */
	free(link);
}

/**
 * Returns the maximum value under the given link, that is, the rightmost link.
 */
link_t* max_link(link_t* link) {
	while (link != NULL && link->right != NULL) link = link->right;
	return link;
}

/**
 * Removes a link from the tree (identified by the root link) by its name.
 * Automatically balances the tree afterwards.
 * Frees the removed link, but not its value.
 * Returns the link that is now in the position of the given link.
 */
link_t* delete_link(link_t* link, char* name) {
	int cmp;
	if (link == NULL) return link;

	cmp = strcmp(name, link->value->name);
	if (cmp < 0)
		link->left = delete_link(link->left, name);
	else if (cmp > 0)
		link->right = delete_link(link->right, name);
	else {
		if (link->left != NULL && link->right != NULL) {
			link_t* aux = max_link(link->left);
			{
				file_t* x = link->value;
				link->value = aux->value;
				aux->value = x;
			}
			link->left = delete_link(link->left, aux->value->name);
		} else {
			link_t* aux = link;
			if (link->left == NULL && link->right == NULL)
				link = NULL;
			else if (link->left == NULL)
				link = link->right;
			else
				link = link->left;
			/* no need to free item since that's handled by the file itself */

			free(aux);
		}
	}
	link = balance(link);
	return link;
}

/* Hashtable */

/**
 * Hash a string based on the size of the hashtable.
 * Returns the generated hash.
 */
int hash_string(char* v, int size) {
	long int hash, a = 31415, b = 27183;

	for (hash = 0; *v != '\0'; v++, a = a * b % (size - 1)) {
		hash = (a * hash + *v) % size;
	}
	return hash;
}

/**
 * Creates a new (empty) hashtable with the given size.
 * The hashtable uses a linear probing strategy.
 * Returns a pointer to the newly created hashtable.
 */
hashtable_t* init_hashtable(int size) {
	hashtable_t* hashtable = (hashtable_t*)malloc(sizeof(hashtable_t));
	int i;
	file_t** file_array = (file_t**)malloc(sizeof(file_t*) * size);

	hashtable->table = file_array;
	hashtable->size = size;
	hashtable->count = 0;

	for (i = 0; i < size; ++i) {
		file_array[i] = NULL;
	}

	return hashtable;
}

/**
 * Inserts a file into the hashtable, hashed by the file value.
 * If the resulting hashtable size is more than half the size of the hashtable,
 * the hashtable is expanded by doubling its size.
 * Returns a pointer to the resulting hashtable, since it can be expanded.
 */
hashtable_t* insert_hashtable(hashtable_t* hashtable, file_t* file) {
	int i = hash_string(file->value, hashtable->size);
	while (hashtable->table[i] != NULL) i = (i + 1) % hashtable->size;
	hashtable->table[i] = file;
	/* dynamically expand hashtable*/
	if (++hashtable->count > hashtable->size / 2)
		hashtable = expand_hashtable(hashtable);
	return hashtable;
}

/**
 * Search for a file by value in the hashtable.
 * Returns the first file with the value if it exists.
 * Otherwise, returns NULL.
 */
file_t* search_hashtable(hashtable_t* hashtable, char* value) {
	int i = hash_string(value, hashtable->size);
	while (hashtable->table[i] != NULL) {
		if (strcmp(hashtable->table[i]->value, value) == 0) {
			return hashtable->table[i];
		} else {
			i = (i + 1) % hashtable->size;
		}
	}
	return NULL;
}

/**
 * Deletes the given file from the hashtable.
 * Does not free the memory associated with the file.
 * The index for the imediately next items is recalculated.
 */
void delete_hashtable(hashtable_t* hashtable, file_t* file) {
	int j, i = hash_string(file->value, hashtable->size);
	file_t* tmp_file;
	while (hashtable->table[i] != NULL) {
		if (hashtable->table[i] == file)
			break; /* compare by pointers */
		else
			i = (i + 1) % hashtable->size;
	}
	if (hashtable->table[i] == NULL) return;
	hashtable->table[i] = NULL;
	--hashtable->count;

	for (j = (i + 1) % hashtable->size; hashtable->table[j] != NULL;
	     j = (j + 1) % hashtable->size) {
		tmp_file = hashtable->table[j];
		hashtable->table[j] = NULL;
		--hashtable->count;
		insert_hashtable(hashtable, tmp_file);
	}
}

/**
 * Destroys the given hashtable, freeing the memory used by it.
 * Does not free the memory associated with the files themselves.
 */
void destroy_hashtable(hashtable_t* hashtable) {
	free(hashtable->table);
	free(hashtable);
}

/**
 * Expands the given hashtable by doubling its size.
 * Creates a new hashtable and recalculates the hash for all the values it
 * holds, inserting them into the new hashtable.
 * Returns the result of expanding the hashtable.
 */
hashtable_t* expand_hashtable(hashtable_t* hashtable) {
	int i;
	hashtable_t* new_hashtable = init_hashtable(hashtable->size * 2);

	for (i = 0; i < hashtable->size; i++) {
		if (hashtable->table[i] != NULL)
			insert_hashtable(new_hashtable, hashtable->table[i]);
	}

	destroy_hashtable(hashtable);

	return new_hashtable;
}
