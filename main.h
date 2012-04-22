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

