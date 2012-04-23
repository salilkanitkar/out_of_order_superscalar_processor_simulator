#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<fcntl.h>

#include "main.h"

void initialize_data_structs(int S, int N)
{
	int i;

	/* Initialize and allocate fake_rob.
	   The fake_rob will be a circular linked list.
	*/
	fake_rob = (node_t *)malloc(sizeof(node_t) * 1);
	if (!fake_rob) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	fake_rob->pc = -1;
	fake_rob->op = -1;
	fake_rob->pipeline_stage = -1;
	fake_rob->dest_reg = -1;
	fake_rob->src1_reg = -1;
	fake_rob->src2_reg = -1;
	fake_rob->tag = -1;

	fake_rob->next = fake_rob;

	/* Initilize and allocate Dispatch_List.
	   The dispatch_list models the Dispatch Queue. So it will contain instructions in either IF or ID stage.
	    The dispatch_list will be of size "2N".
	*/
	dispatch_list = (node_t *)malloc(sizeof(node_t) * 2 * N);
	if (!dispatch_list) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	for (i=0 ; i < 2*N ; i++) {
		dispatch_list[i].pc = -1;
		dispatch_list[i].op = -1;
		dispatch_list[i].pipeline_stage = -1;
		dispatch_list[i].dest_reg = -1;
		dispatch_list[i].src1_reg = -1;
		dispatch_list[i].src2_reg = -1;
		dispatch_list[i].tag = -1;
		dispatch_list[i].next = 0;
	}

	dispatch_iter = 0;

	/* Initilize and allocate Issue_List.
	   The issue_list models the Scheduling Queue.
	   It contains the list of instructions in IS stage. These will be those waiting for operands or available issue bandwidth.
	   The issue_list will be of size "S".
	*/
	issue_list = (node_t *)malloc(sizeof(node_t) * S);
	if (!issue_list) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	for (i=0 ; i < S ; i++) {
		issue_list[i].pc = -1;
		issue_list[i].op = -1;
		issue_list[i].pipeline_stage = -1;
		issue_list[i].dest_reg = -1;
		issue_list[i].src1_reg = -1;
		issue_list[i].src2_reg = -1;
		issue_list[i].tag = -1;
		issue_list[i].next = 0;
	}

	issue_iter = 0;

	/* Initilize and allocate Execute_List. 
	   The execute_list models the Scheduling Queue. The execute_list models the N FUs (Fully pipelined Functional Units).
	   It contains the list of instructions in EX stage. These will be those waiting for execution latency of the operation.
	   The issue_list will be of size "N".
 	*/
	execute_list = (node_t *)malloc(sizeof(node_t) * N);
	if (!execute_list) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	for (i=0 ; i < N ; i++) {
		execute_list[i].pc = -1;
		execute_list[i].op = -1;
		execute_list[i].pipeline_stage = -1;
		execute_list[i].dest_reg = -1;
		execute_list[i].src1_reg = -1;
		execute_list[i].src2_reg = -1;
		execute_list[i].tag = -1;
		execute_list[i].next = 0;
	}

	execute_iter = 0;
}

