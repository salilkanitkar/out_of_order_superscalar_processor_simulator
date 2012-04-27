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

void print_usage();

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

		inst_count += 1;
	}

	i = 0;

#ifdef DEBUG_FLAG
	for (i=0 ; i < inst_count ; i++) {
		printf("%x %d %d %d %d\n", inst_stream[i].pc, inst_stream[i].op, inst_stream[i].dest_reg, inst_stream[i].src1_reg, inst_stream[i].src2_reg);
	}
#endif
	i = 0;
	do {

		/* DISPATCH */
		dispatch(i);

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

	//} while (advance_cycle(&i));
	} while (i < inst_count); 

//#ifdef DEBUG_FLAG
	print_fake_rob();
	print_dispatch_list();
	print_issue_list();
	print_execute_list();
//#endif
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

