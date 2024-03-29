#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<fcntl.h>

#include "main.h"

int S=0;
int N=0;
FILE *fp_trace;

char trace_file[MAX_TRACEFILE_NAME_LEN];
char trace_str[MAX_TRACESTR_LEN];

inst_t inst;
inst_t inst_stream[MAX_TRACEFILE_SIZE];
unsigned int inst_count=0;

node_t *fake_rob=0;
node_t *dispatch_list=0;
node_t *issue_list=0;
node_t *execute_list=0;

int dispatch_count=0;
int issue_count=0;
int execute_count=0;

register_file_t *register_file=0;

unsigned int proc_cycle=0;
timing_info_t *timing_info=0;

void print_usage();
void print_results();

int main(int argc, char *argv[])
{

	int i=0, j=0;

	if (argc != 4) {
		printf("Invalid Number of arguments!\n");
		print_usage();
		printf("Exiting...\n");
		exit(1);
	}

	S = atoi(argv[1]);
	N = atoi(argv[2]);
	strcpy(trace_file, argv[3]);

	fp_trace = fopen(trace_file, "r");
	if (trace_file == NULL) {
		printf("Error while opening the trace file.\n");
		printf("Exiting...\n");
		exit(1);
	}

	initialize_data_structs(S, N);

	while (fgets(trace_str, MAX_TRACESTR_LEN, fp_trace)) {

		sscanf(trace_str, "%x %d %d %d %d\n", &inst.pc, &inst.op, &inst.dest_reg, &inst.src1_reg, &inst.src2_reg);

#ifdef DEBUG_FLAG
		printf("%x %d %d %d %d\n", inst.pc, inst.op, inst.dest_reg, inst.src1_reg, inst.src2_reg);
#endif
		inst_stream[inst_count].pc = inst.pc;
		inst_stream[inst_count].op = inst.op;
		inst_stream[inst_count].dest_reg = inst.dest_reg;
		inst_stream[inst_count].src1_reg = inst.src1_reg;
		inst_stream[inst_count].src2_reg = inst.src2_reg;
		inst_stream[inst_count].tag = inst_count;

		if (inst_stream[inst_count].op == OPTYPE0) {
			inst_stream[inst_count].op_latency = OPTYPE0_LATENCY;
		} else if (inst_stream[inst_count].op == OPTYPE1) {
			inst_stream[inst_count].op_latency = OPTYPE1_LATENCY;
		} else if (inst_stream[inst_count].op == OPTYPE2) {
			inst_stream[inst_count].op_latency = OPTYPE2_LATENCY;
		}

		inst_count += 1;
	}

	initialize_timing_info(inst_count);

	i = 0;

#ifdef DEBUG_FLAG
	for (i=0 ; i < inst_count ; i++) {
		printf("%x %d %d %d %d\n", inst_stream[i].pc, inst_stream[i].op, inst_stream[i].dest_reg, inst_stream[i].src1_reg, inst_stream[i].src2_reg);
	}
#endif
	i = 0;
	do {

		/* FAKE_RETIRE */
		fake_retire();
#ifdef DEBUG_FLAG
		printf("Cycle %d: FakeRetire Done.\n", proc_cycle);
		print_fake_rob();
		print_dispatch_list();
		print_issue_list();
		print_execute_list();
		getchar();
#endif

		/* EXECUTE */
		execute();
#ifdef DEBUG_FLAG
		printf("Cycle %d: Execute Done.\n", proc_cycle);
		print_fake_rob();
		print_dispatch_list();
		print_issue_list();
		print_execute_list();
		getchar();
#endif

		/* ISSUE */
		issue();
#ifdef DEBUG_FLAG
		printf("Cycle %d: Issue Done.\n", proc_cycle);
		print_fake_rob();
		print_dispatch_list();
		print_issue_list();
		print_execute_list();
		getchar();
#endif

		/* DISPATCH */
		dispatch();
#ifdef DEBUG_FLAG
		printf("Cycle %d: Dispatch Done.\n", proc_cycle);
		print_fake_rob();
		print_dispatch_list();
		print_issue_list();
		print_execute_list();
		getchar();
#endif

		/* FETCH */
		/* Keep on fetching new instructions until ALL of the following is TRUE - 
		   1) Instruction Stream is no exhausted.
		   2) The fetch bandwidth (N) is not exhausted.
		   3) The dispatch_list is not full.
		*/
		j = 0;
		while (i < inst_count && j < N && dispatch_count < (2*N)) {
			do_fetch(&inst_stream[i]);
			i += 1;
			j += 1;
		}
#ifdef DEBUG_FLAG
		printf("Cycle %d: Fetch Done.\n", proc_cycle);
		print_fake_rob();
		print_dispatch_list();
		print_issue_list();
		print_execute_list();
		getchar();
#endif

	} while (advance_cycle(&i));
	//} while (i < inst_count); 

#ifdef DEBUG_FLAG
	print_fake_rob();
	print_dispatch_list();
	print_issue_list();
	print_execute_list();
#endif

	print_results();
	return(0);
}

void print_usage()
{
	printf("\nThis Out-of-order Superscalar Processor Simulator should be run as follows - \n");
	printf("$ ./sim S N tracefile\n");
	printf("Where:\n");
	printf("S: The Scheduling Queue Size\n");
	printf("N: The peak fetch, issue and disptach rate.\n");
	printf("tracefile: Filename of the input trace\n\n");
}

void print_results()
{
	proc_cycle -= 1;
	float ipc = ((float)inst_count) / ((float)proc_cycle);

	printf("CONFIGURATION\n");
	printf(" superscalar bandwidth (N) = %d\n", N);
	printf(" dispatch queue size (2*N) = %d\n", 2*N);
	printf(" schedule queue size (S)   = %d\n", S);
	printf("RESULTS\n");
	printf(" number of instructions = %d\n", inst_count);
	printf(" number of cycles       = %d\n", proc_cycle);
	printf(" IPC                    = %.2f\n", ipc);
}

