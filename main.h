#define TRUE 1
#define FALSE 0

#define MAX_TRACEFILE_NAME_LEN 200
#define MAX_TRACESTR_LEN 100

#define OPTYPE0 0
#define OPTYPE1 1
#define OPTYPE2 2

#define READY 1
#define NOT_READY 0

#define MAX_TRACEFILE_SIZE 1<<20

extern int S;
extern int N;
extern FILE *fp_trace;
extern char trace_file[MAX_TRACEFILE_NAME_LEN];
extern char trace_str[MAX_TRACESTR_LEN];

typedef struct _inst_t {
	unsigned int pc;
	int op;
	int dest_reg;
	int src1_reg;
	int src2_reg;

	unsigned int tag;
}inst_t;

extern inst_t inst;
extern inst_t inst_stream[MAX_TRACEFILE_SIZE];
extern unsigned int inst_count;

#define IF 0
#define ID 1
#define IS 2
#define EX 3
#define WB 4

typedef struct _node_t {
	unsigned int pc;
	int op;
	int pipeline_stage;

	int dest_reg;
	int src1_reg;
	int src2_reg;

	int dest_ready;
	int src1_ready;
	int src2_ready;

	int dest_val;
	int src1_val;
	int src2_val;

	unsigned int tag;

	struct _node_t *next;
}node_t;

extern node_t *fake_rob;
extern node_t *dispatch_list;
extern node_t *issue_list;
extern node_t *execute_list;

extern int dispatch_count;
extern int issue_count;
extern int execute_count;

#define REGISTER_FILE_SIZE 128

typedef struct _register_file_t {
	int ready;
	unsigned int tag_value;
}register_file_t;

extern register_file_t *register_file;

extern void initialize_data_structs(int, int);

extern unsigned int proc_cycle;

extern int advance_cycle(int *);
extern void do_fetch(inst_t *);
extern void dispatch(int);

extern void print_fake_rob();
extern void print_dispatch_list();
extern void print_issue_list();
extern void print_execute_list();

