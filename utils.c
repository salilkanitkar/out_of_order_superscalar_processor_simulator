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
	   The dispatch_list will be a circular linked list with a head node. 
	   The "dispatch_count" keeps track of number instructions in the dispatch_list.
	*/
	dispatch_list = (node_t *)malloc(sizeof(node_t) * 1);
	if (!dispatch_list) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	dispatch_list->pc = -1;
	dispatch_list->op = -1;
	dispatch_list->pipeline_stage = -1;
	dispatch_list->dest_reg = -1;
	dispatch_list->src1_reg = -1;
	dispatch_list->src2_reg = -1;
	dispatch_list->tag = -1;

	dispatch_list->next = dispatch_list;

	dispatch_count = 0;

	/* Initilize and allocate Issue_List.
	   The issue_list models the Scheduling Queue.
	   It contains the list of instructions in IS stage. These will be those waiting for operands or available issue bandwidth.
	   The issue_list will be of size "S".
	   The issue_list will be a circular linked list with a head node. 
	   The "issue_count" keeps track of number instructions in the issue_list.
	*/
	issue_list = (node_t *)malloc(sizeof(node_t) * 1);
	if (!issue_list) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	issue_list->pc = -1;
	issue_list->op = -1;
	issue_list->pipeline_stage = -1;
	issue_list->dest_reg = -1;
	issue_list->src1_reg = -1;
	issue_list->src2_reg = -1;
	issue_list->tag = -1;

	issue_list->next = issue_list;

	issue_count = 0;

	/* Initilize and allocate Execute_List. 
	   The execute_list models the Scheduling Queue. The execute_list models the N FUs (Fully pipelined Functional Units).
	   It contains the list of instructions in EX stage. These will be those waiting for execution latency of the operation.
	   The execute_list will be of size "N".
	   The execute_list will be a circular linked list with a head node. 
	   The "execute_count" keeps track of number instructions in the execute_list.
 	*/
	execute_list = (node_t *)malloc(sizeof(node_t) * 1);
	if (!execute_list) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	execute_list->pc = -1;
	execute_list->op = -1;
	execute_list->pipeline_stage = -1;
	execute_list->dest_reg = -1;
	execute_list->src1_reg = -1;
	execute_list->src2_reg = -1;
	execute_list->tag = -1;

	execute_list->next = execute_list;

	execute_count = 0;

	/* The Register File is modeled by register_file struct.
	   The REGISTER_FILE_SIZE determines the size of the register_file.
	   The tag_value field models both the tag or value held in that register -  as per Tomasulo's Algorithm.
	   If ready bit is set to READY, then the tag_value field contains the value i.e. index. 
	   If ready bit is set to NOT_READY, then the tag_value field contains the tag - part of Instruction's State. 
 	*/
	register_file = (register_file_t *)malloc(sizeof(register_file_t) * REGISTER_FILE_SIZE);
	if (!register_file) {
		printf("Error in allocating memory!\n");
		printf("Exiting...\n");
		exit(1);
	}

	/* Initialize ready bit to READY.
	   So the tag_value should represent value i.e. index in the register_file.
	*/
	for (i=0 ; i < REGISTER_FILE_SIZE; i++) {
		register_file[i].ready = READY;
		register_file[i].tag_value = i;
	}

	/* The inst_stream represents the Instruction Stream. 
	*/
	for (i=0 ; i < MAX_TRACEFILE_SIZE ; i++) {
		inst_stream[i].pc = -1;
		inst_stream[i].op = -1;
		inst_stream[i].dest_reg = -1;
		inst_stream[i].src1_reg = -1;
		inst_stream[i].src2_reg = -1;
		inst_stream[i].tag = -1;
	}
}

