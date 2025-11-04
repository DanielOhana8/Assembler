#define max_file_name 256
#define cmd_num 20
#define max_line_len 80
#define max_macro_name 32
#define max_macro_val 20*max_line_len
#define mem_size 4096
#define chars_of_mem 4
#define word_bits 15

typedef struct node
{
	char* name;
	char* val;
	int num;
	struct node* next; /* Pointer to the next node in the list */
} node;

