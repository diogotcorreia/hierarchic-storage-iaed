/* Diogo Torres Correia, ist199211 */

#include "structures.h"

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
#define DELETE_ERR_PATH_NOT_FOUND "not found\n"
#define SEARCH_ERR_PATH_NOT_FOUND "not found\n"

typedef struct file {
	struct file* parent;
	char* name;
	char* value;
	list_t* children_by_creation;
	link_t* children_tree;
} file_t;

typedef struct {
	file_t* root_file;
	hashtable_t* search_table;
} storage_t;

/***********/
/* proj2.c */
/***********/

short handle_command(storage_t* storage, char* command);

void handle_help_command();

void handle_set_command(storage_t* storage, char* arguments);

void handle_print_command(storage_t* storage);

void handle_find_command(storage_t* storage, char* arguments);

void handle_list_command(storage_t* storage, char* arguments);

void handle_search_command(storage_t* storage, char* arguments);

void handle_delete_command(storage_t* storage, char* arguments);

/* utils */

short is_whitespace(char c);

char* trim_whitespace(char* str);

/***********/
/* paths.c */
/***********/

void add_path_recursively(storage_t* storage, char* path, char* value);

file_t* get_file_by_path(storage_t* storage, char* path);

file_t* init_file(file_t* parent, char* name);

void set_file_value(storage_t* storage, file_t* file, char* value);

list_t* get_file_children_by_creation(file_t* parent);

file_t* get_child_by_name(file_t* parent, char* name);

void destroy_file(void* storage, void* file);

void delete_file(storage_t* storage, file_t* file);

void print_files_recursively(void* data, void* value);

void print_file_path(file_t* file);

void print_file_tree(link_t* link);

/* auxiliaries */

char* get_file_name(void* file);

char* get_file_value(void* file);

void print_file_name(void* file);

#endif