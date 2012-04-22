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

void print_usage();

int main(int argc, char *argv[])
{
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

	while (fgets(trace_str, MAX_TRACESTR_LEN, fp_trace)) {

		sscanf(trace_str, "%x %d %d %d %d\n", &inst.pc, &inst.op, &inst.dest_reg, &inst.src1_reg, &inst.src2_reg);

		printf("%x %d %d %d %d\n", inst.pc, inst.op, inst.dest_reg, inst.src1_reg, inst.src2_reg);
	}

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

