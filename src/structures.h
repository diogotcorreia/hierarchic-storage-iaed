/* Diogo Torres Correia, ist199211 */

#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#define ERR_NO_MEMORY "No memory.\n"
#define EXIT_CODE_NO_MEMORY -1

#define HASHTABLE_START_SIZE 7

typedef struct node node_t;
typedef struct list list_t;

typedef struct link link_t;

typedef struct hashtable hashtable_t;

/* linked lists */

list_t* init_list();

void insert_list(list_t* list, void* value);

void destroy_list(list_t* list);

void delete_node(list_t* list, void* value);

void traverse_list(list_t* list, void* data,
                   void (*fn)(void* data, void* value));

/* trees */

link_t* init_link(void* value, link_t* left, link_t* right);

int height(link_t* link);

void update_height(link_t* link);

link_t* rotL(link_t* link);
link_t* rotR(link_t* link);
link_t* rotLR(link_t* link);
link_t* rotRL(link_t* link);

int balance_factor(link_t* link);

link_t* balance(link_t* link);

link_t* insert_tree(link_t* link, void* value, char* (*key)(void*));

void* get_link_by_value(link_t* link, char* value, char* (*key)(void*));

void destroy_tree(link_t* link);

link_t* max_link(link_t* link);

link_t* delete_link(link_t* link, char* value, char* (*key)(void*));

void traverse_tree(link_t* link, void (*fn)(void* value));

/* hashtables */

int hash_string(char* v, int size);

hashtable_t* init_hashtable(int size);

hashtable_t* insert_hashtable(hashtable_t* hashtable, void* value,
                              char* (*key)(void*));

void* search_hashtable(hashtable_t* hashtable, char* value,
                       char* (*key)(void*));

void delete_hashtable(hashtable_t* hashtable, void* value, char* (*key)(void*));

void destroy_hashtable(hashtable_t* hashtable);

hashtable_t* expand_hashtable(hashtable_t* hashtable, char* (*key)(void*));

/* utils */

void swap_variables(void** a, void** b);

void* safe_malloc(unsigned int size);

#endif