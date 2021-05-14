/* Diogo Torres Correia, ist199211 */

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

struct file;

typedef struct node {
	struct node* next;
	struct node* prev;
	struct file* value;
} node_t;

typedef struct list {
	node_t* first;
	node_t* last;
} list_t;

typedef struct file {
	struct file* parent;
	char* name;
	char* value;
	list_t* children_by_creation;
	/* TODO add AVL binary tree */
} file_t;

typedef struct {
	file_t* root_file;
} storage_t;

/* proj2.c */

int handle_command(storage_t* storage);

void handle_help_command();

void handle_set_command(storage_t* storage, char* arguments);

/* paths.c */

void add_path_recursively(storage_t* storage, char* path, char* value);

file_t* init_file(file_t* parent, char* name);

void set_file_value(file_t* file, char* value);

list_t* get_file_children_by_creation(file_t* parent);

void destroy_file(file_t* file);

list_t* init_list();

void insert_list(list_t* list, file_t* node);

void destroy_list(list_t* list);

/* utils.c */

short is_whitespace(char c);

char* trim_whitespace(char* str);
