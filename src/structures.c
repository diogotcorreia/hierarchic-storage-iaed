/* Diogo Torres Correia, ist199211 */

#include "structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
	struct node* next;
	struct node* prev;
	void* value;
};

struct list {
	node_t* first;
	node_t* last;
};

struct link {
	void* value;
	struct link* left;
	struct link* right;
	int height;
};

struct hashtable {
	void** table;
	int count;
	int size;
};

/* Linked List */

/**
 * Create a new empty double linked list.
 * Returns the newly created list.
 */
list_t* init_list() {
	list_t* new_list = (list_t*)safe_malloc(sizeof(list_t));
	new_list->first = NULL;
	new_list->last = NULL;
	return new_list;
}

/**
 * Inserts a given (already allocated) value into the given double linked list,
 * as the last element.
 */
void insert_list(list_t* list, void* item) {
	node_t* node = (node_t*)safe_malloc(sizeof(node_t));

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
 * Removes all items from the list, frees the list nodes and the associated
 * values.
 */
void destroy_list(list_t* list) {
	node_t* next = list->first;

	while (next != NULL) {
		node_t* aux = next;
		next = aux->next;

		free(aux);
	}

	free(list);
}

/**
 * Deletes a single node from the list that has the given value, freeing
 * itself. The value is not freed.
 */
void delete_node(list_t* list, void* value) {
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

void traverse_list_reverse(list_t* list, void* data,
                           void (*fn)(void* data, void* value)) {
	node_t* aux = list->last;
	while (aux != NULL) {
		fn(data, aux->value);
		aux = aux->prev;
	}
}

/* AVL Tree */

/**
 * Creates a new AVL Tree item with the given item and left/right children.
 * Returns the newly created link.
 */
link_t* init_link(void* value, link_t* left, link_t* right) {
	link_t* new_link = (link_t*)safe_malloc(sizeof(link_t));

	new_link->value = value;
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
 * Insert a new value under the given link.
 * Automatically balances the resulting AVL tree.
 */
link_t* insert_tree(link_t* link, void* value, char* (*key)(void*)) {
	if (link == NULL) return init_link(value, NULL, NULL);

	if (strcmp(key(value), key(link->value)) < 0)
		link->left = insert_tree(link->left, value, key);
	else
		link->right = insert_tree(link->right, value, key);
	link = balance(link);
	return link;
}

/**
 * Get the item that identified by the given name (value) from the given tree
 * (link).
 * Returns the found value. If the value does not exist, returns NULL.
 */
void* get_link_by_value(link_t* link, char* value, char* (*key)(void*)) {
	int cmp;
	if (link == NULL) return NULL;

	cmp = strcmp(value, key(link->value));
	if (cmp == 0) {
		return link->value;
	} else if (cmp < 0) {
		return get_link_by_value(link->left, value, key);
	} else {
		return get_link_by_value(link->right, value, key);
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
 * Removes a link from the tree (identified by the root link) by a compare
 * function with a given value.
 * Automatically balances the tree afterwards.
 * Frees the removed link, but not its value.
 * Returns the link that is now in the position of the given link.
 */
link_t* delete_link(link_t* link, char* value, char* (*key)(void*)) {
	int cmp;
	if (link == NULL) return link;

	cmp = strcmp(value, key(link->value));
	if (cmp < 0)
		link->left = delete_link(link->left, value, key);
	else if (cmp > 0)
		link->right = delete_link(link->right, value, key);
	else {
		if (link->left != NULL && link->right != NULL) {
			link_t* aux = max_link(link->left);
			swap_variables(&link->value, &aux->value);
			link->left = delete_link(link->left, key(aux->value), key);
		} else {
			link_t* aux = link;
			if (link->left == NULL && link->right == NULL)
				link = NULL;
			else if (link->left == NULL)
				link = link->right;
			else
				link = link->left;

			free(aux);
		}
	}
	link = balance(link);
	return link;
}

void traverse_tree_reverse(link_t* link, void (*fn)(void* value)) {
	if (link == NULL) return;

	traverse_tree_reverse(link->right, fn);
	fn(link->value);
	traverse_tree_reverse(link->left, fn);
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
	hashtable_t* hashtable = (hashtable_t*)safe_malloc(sizeof(hashtable_t));
	int i;
	void** table = (void**)safe_malloc(sizeof(void*) * size);

	hashtable->table = table;
	hashtable->size = size;
	hashtable->count = 0;

	for (i = 0; i < size; ++i) {
		table[i] = NULL;
	}

	return hashtable;
}

/**
 * Inserts an object into the hashtable, hashed by its (non-unique) key.
 * If the resulting hashtable size is more than half the size of the hashtable,
 * the hashtable is expanded by doubling its size.
 * Returns a pointer to the resulting hashtable, since it can be expanded.
 */
hashtable_t* insert_hashtable(hashtable_t* hashtable, void* value,
                              char* (*key)(void*)) {
	int i = hash_string(key(value), hashtable->size);
	while (hashtable->table[i] != NULL) i = (i + 1) % hashtable->size;
	hashtable->table[i] = value;
	/* dynamically expand hashtable*/
	if (++hashtable->count > hashtable->size / 2)
		hashtable = expand_hashtable(hashtable, key);
	return hashtable;
}

/**
 * Search for an object by value in the hashtable.
 * Returns the first object with the value if it exists.
 * Otherwise, returns NULL.
 */
void* search_hashtable(hashtable_t* hashtable, char* value,
                       char* (*key)(void*)) {
	int i = hash_string(value, hashtable->size);
	while (hashtable->table[i] != NULL) {
		if (strcmp(key(hashtable->table[i]), value) == 0) {
			return hashtable->table[i];
		} else {
			i = (i + 1) % hashtable->size;
		}
	}
	return NULL;
}

/**
 * Deletes the given object from the hashtable.
 * Does not free the memory associated with the object.
 * The index for the imediately next objects is recalculated.
 */
void delete_hashtable(hashtable_t* hashtable, void* value,
                      char* (*key)(void*)) {
	int j, i = hash_string(key(value), hashtable->size);
	void* tmp;
	while (hashtable->table[i] != NULL) {
		if (hashtable->table[i] == value)
			break; /* compare by pointers */
		else
			i = (i + 1) % hashtable->size;
	}
	if (hashtable->table[i] == NULL) return;
	hashtable->table[i] = NULL;
	--hashtable->count;

	for (j = (i + 1) % hashtable->size; hashtable->table[j] != NULL;
	     j = (j + 1) % hashtable->size) {
		tmp = hashtable->table[j];
		hashtable->table[j] = NULL;
		--hashtable->count;
		insert_hashtable(hashtable, tmp, key);
	}
}

/**
 * Destroys the given hashtable, freeing the memory used by it.
 * Does not free the memory associated with the objects themselves.
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
hashtable_t* expand_hashtable(hashtable_t* hashtable, char* (*key)(void*)) {
	int i;
	hashtable_t* new_hashtable = init_hashtable(hashtable->size * 2);

	for (i = 0; i < hashtable->size; i++) {
		if (hashtable->table[i] != NULL)
			insert_hashtable(new_hashtable, hashtable->table[i], key);
	}

	destroy_hashtable(hashtable);

	return new_hashtable;
}

/* utils */

void swap_variables(void** a, void** b) {
	void* c = *a;
	*a = *b;
	*b = c;
}

void* safe_malloc(unsigned int size) {
	void* p = malloc(size);
	if (p != NULL) return p;

	printf(ERR_NO_MEMORY);
	exit(EXIT_CODE_NO_MEMORY);
	return NULL;
}
