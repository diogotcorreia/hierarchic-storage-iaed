/* Diogo Torres Correia, ist199211 */

#ifndef _PROJ2_H_
#define _PROJ2_H_

#define MAX_COMMAND_SIZE 65536
#define HELP_CMD "help"
#define HELP_DESC "Imprime os comandos disponíveis."
#define QUIT_CMD "quit"
#define QUIT_DESC "Termina o programa."
#define SET_CMD "set"
#define SET_DESC "Adiciona ou modifica o valor a armazenar."
#define PRINT_CMD "print"
#define PRINT_DESC "Imprime todos os caminhos e valores."
#define FIND_CMD "find"
#define FIND_DESC "Imprime o valor armazenado."
#define LIST_CMD "list"
#define LIST_DESC "Lista todos os componentes imediatos de um sub-caminho."
#define SEARCH_CMD "search"
#define SEARCH_DESC "Procura o caminho dado um valor."
#define DELETE_CMD "delete"
#define DELETE_DESC "Apaga um caminho e todos os subcaminhos."

#define WHITESPACE " \t\n"
#define PATH_SEPARATOR "/"

#define HELP_COMMAND_FORMAT "%s: %s\n"
#define PATH_PRINT_FORMAT "/%s"
#define PATH_VALUE_FORMAT " %s\n"
#define FIND_FORMAT "%s\n"
#define LIST_FORMAT "%s\n"

#define FIND_ERR_FILE_NOT_FOUND "not found\n"
#define FIND_ERR_FILE_EMPTY "no data\n"
#define LIST_ERR_PATH_NOT_FOUND "not found\n"

struct file;

typedef struct node {
	struct node* next;
	struct node* prev;
	struct file* value;
} node_t;

typedef struct link {
	struct file* value;
	struct link* left;
	struct link* right;
	int height;
} link_t;

typedef struct list {
	node_t* first;
	node_t* last;
} list_t;

typedef struct file {
	struct file* parent;
	char* name;
	char* value;
	list_t* children_by_creation;
	link_t* children_tree;
} file_t;

typedef struct {
	file_t* root_file;
} storage_t;

/***********/
/* proj2.c */
/***********/

int handle_command(storage_t* storage);

void handle_help_command();

void handle_set_command(storage_t* storage, char* arguments);

void handle_print_command(storage_t* storage);

void print_file_recursively(file_t* file);

void print_file_path(file_t* file);

void handle_find_command(storage_t* storage, char* arguments);

void handle_list_command(storage_t* storage, char* arguments);

void print_file_tree(link_t* link);

/***********/
/* paths.c */
/***********/

void add_path_recursively(storage_t* storage, char* path, char* value);

file_t* get_file_by_path(storage_t* storage, char* path);

/* files */

file_t* init_file(file_t* parent, char* name);

void set_file_value(file_t* file, char* value);

list_t* get_file_children_by_creation(file_t* parent);

file_t* get_child_by_name(file_t* parent, char* name);

void destroy_file(file_t* file);

void delete_file(file_t* file);

/* linked lists */

list_t* init_list();

void insert_list(list_t* list, file_t* file);

void destroy_list(list_t* list);

void delete_node(list_t* list, file_t* value);

/* trees */

link_t* init_link(file_t* file, link_t* left, link_t* right);

int height(link_t* link);

void update_height(link_t* link);

link_t* rotL(link_t* link);

link_t* rotR(link_t* link);

link_t* rotLR(link_t* link);

link_t* rotRL(link_t* link);

int balance_factor(link_t* link);

link_t* balance(link_t* link);

link_t* insert_tree(link_t* link, file_t* file);

file_t* get_link_by_value(link_t* link, char* value);

void destroy_tree(link_t* link);

link_t* max_link(link_t* link);

link_t* delete_link(link_t* link, char* name);

/***********/
/* utils.c */
/***********/

short is_whitespace(char c);

char* trim_whitespace(char* str);

#endif