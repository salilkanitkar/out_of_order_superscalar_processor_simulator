#define TRUE 0
#define FALSE 1

#define MAX_TRACEFILE_NAME_LEN 200
#define MAX_TRACESTR_LEN 100

#define OPTYPE0 0
#define OPTYPE1 1
#define OPTYPE2 2

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
}inst_t;

extern inst_t inst;

extern unsigned int tag;

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

	unsigned int tag;

	struct _node_t *next;
}node_t;

extern node_t *fake_rob;
extern node_t *dispatch_list;
extern node_t *issue_list;
extern node_t *execute_list;

extern int dispatch_iter;
extern int issue_iter;
extern int execute_iter;

extern void initialize_data_structs(int, int);

