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

#define HELP_COMMAND_FORMAT "%s: %s\n"

typedef struct {
} storage;

int handle_command(/*storage *global_store*/);

void handle_help_command();
