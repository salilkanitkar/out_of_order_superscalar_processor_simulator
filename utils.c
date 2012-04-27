#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<fcntl.h>

#include "main.h"

void init_node(node_t *node)
{
	node->pc = -1;
	node->op = -1;

	node->pipeline_stage = -1;

	node->dest_reg = -1;
	node->src1_reg = -1;
	node->src2_reg = -1;

	node->tag = -1;

	node->dest_ready = NOT_READY;
	node->src1_ready = NOT_READY;
	node->src2_ready = NOT_READY;

	node->dest_val = node->dest_reg;
	node->src1_val = node->src1_reg;
	node->src2_val = node->src2_reg;

	node->op_latency = -1;
}

void copy_node(node_t *dest, node_t *src)
{
	dest->pc = src->pc;
	dest->op = src->op;

	dest->pipeline_stage = src->pipeline_stage;

	dest->dest_reg = src->dest_reg;
	dest->src1_reg = src->src1_reg;
	dest->src2_reg = src->src2_reg;

	dest->tag = src->tag;

	dest->dest_ready = src->dest_ready;
	dest->src1_ready = src->src1_ready;
	dest->src2_ready = src->src2_ready;

	dest->dest_val = src->dest_val;
	dest->src1_val = src->src1_val;
	dest->src2_val = src->src2_val;

	dest->op_latency = src->op_latency;
}

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

	init_node(fake_rob);
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

	init_node(dispatch_list);
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

	init_node(issue_list);
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

	init_node(execute_list);
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

void sort_list(node_t *node_list, int count)
{
	/* Insertion Sort - Ascending Order based on tag. */
	int i=0, j=0;
	node_t x;

	for (j=1 ; j < count ; j++) {
		copy_node(&x, &node_list[j]);
		i = j - 1;
		while (i >= 0 && node_list[i].tag > x.tag) {
			copy_node(&node_list[i+1], &node_list[i]);
			i -= 1;
		}
		copy_node(&node_list[i+1], &x);
	}
}

int advance_cycle(int *i)
{
	/* Advance the Processor Cycle. This models a clock-tick event. */
	proc_cycle += 1;

	/* When fake_rob is empty AND trace is depleted, return FALSE else return TRUE. */
	if (*i >= inst_count && fake_rob->next == fake_rob) {
		return FALSE;
	} else if (*i < inst_count && fake_rob->next == fake_rob){
		(*i) += 1;
		return TRUE;
	} else if (*i >= inst_count && fake_rob->next != fake_rob) {
		return TRUE;
	} else if (*i < inst_count && fake_rob->next != fake_rob) {
		(*i) += 1;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void do_fetch(inst_t *inst)
{
	node_t *p, *tmp;

	/* Put the new instruction in fake_rob. */
	p = (node_t *)malloc(sizeof(node_t) * 1);
	if (!p) {
		printf("Memory Allocation Failed!\n");
		printf("Exiting...\n");
		exit(1);
	}

	p->pc = inst->pc;
	p->op = inst->op;
	p->dest_reg = inst->dest_reg;
	p->src1_reg = inst->src1_reg;
	p->src2_reg = inst->src2_reg;
	p->tag = inst->tag;
	p->dest_ready = NOT_READY;
	p->src1_ready = NOT_READY;
	p->src2_ready = NOT_READY;
	p->dest_val = p->dest_reg;
	p->src1_val = p->src1_reg;
	p->src2_val = p->src2_reg;
	p->op_latency = inst->op_latency;
	p->pipeline_stage = IF;

	tmp = fake_rob;
	while (tmp->next != fake_rob) {
		tmp = tmp->next;
	}

	tmp->next = p;
	p->next = fake_rob;
		
	/* Add instruction to the dispatch_list. */
	p = (node_t *)malloc(sizeof(node_t) * 1);
	if (!p) {
		printf("Memory allocation Failed!\n");
		printf("Exiting...\n");
		exit(1);
	}

	p->pc = inst->pc;
	p->op = inst->op;
	p->dest_reg = inst->dest_reg;
	p->src1_reg = inst->src1_reg;
	p->src2_reg = inst->src2_reg;
	p->tag = inst->tag;
	p->dest_ready = NOT_READY;
	p->src1_ready = NOT_READY;
	p->src2_ready = NOT_READY;
	p->dest_val = p->dest_reg;
	p->src1_val = p->src1_reg;
	p->src2_val = p->src2_reg;
	p->op_latency = inst->op_latency;
	p->pipeline_stage = IF;

	tmp = dispatch_list;
	while (tmp->next != dispatch_list) {
		tmp = tmp->next;
	}
	tmp->next = p;
	p->next = dispatch_list;
		
	/* Reserve the dispatch Queue entry. */
	dispatch_count += 1;

}

void dispatch()
{
	int i=0, temp_count=0;

	node_t *temp_list=0;
	node_t *p=0, *q=0, *tmp=0;

	/* From the dispatch_list, construct a temporary list of instructions in the ID state.
	*/
	temp_list = (node_t *)malloc(sizeof(node_t) * dispatch_count);
	if (!temp_list) {
		printf("Memory allocation failed!\n");
		printf("Exiting...\n");
		exit(1);
	}

	for (i=0 ; i < dispatch_count ; i++) {
		init_node(&temp_list[i]);
	}

	tmp = dispatch_list->next;
	temp_count = 0;

	for (i=0 ; i < dispatch_count ; i++) {
		if (tmp->pipeline_stage == ID) {
			copy_node(&temp_list[temp_count], tmp);
			temp_count += 1;
		}
		tmp = tmp->next;
	}

	/* Sort the temp_list in ascending order of tags. */
	sort_list(temp_list, temp_count);

	i = 0;
	while (i < temp_count && issue_count < S) {
		/* Remove the instruction from dispatch_list. */
		p = dispatch_list->next;
		q = NULL;
		while (p != dispatch_list) {
			if (p->tag == temp_list[i].tag) {
				if (!q) {
					dispatch_list->next = p->next;
				} else {
					q->next = p->next;
				}
				break;
			}
			q = p;
			p = p->next;
		}

		/* Add the removed entry to issue_list. */
		tmp = issue_list;
		while (tmp->next != issue_list)
			tmp = tmp->next;
		tmp->next = p;
		p->next = issue_list;

		/* Reserve a Schedule Queue Entry. */
		issue_count += 1;
		/* Free a Dispatch Queue Entry. */
		dispatch_count -= 1;

		/* Change pipeline stage from ID to IS. */
		p->pipeline_stage = IS;
		tmp = fake_rob->next;
		while (tmp != fake_rob) {
			if (tmp->tag == p->tag) {
				tmp->pipeline_stage = IS;
				break;
			}
			tmp = tmp->next;
		}

		/* Rename Source Operand1 */
		if (register_file[p->src1_reg].ready == READY) {
			p->src1_ready = READY;
			p->src1_val = register_file[p->src1_reg].tag_value;
		} else if (register_file[p->src1_reg].ready == NOT_READY){
			p->src1_ready = NOT_READY;
			p->src1_val = register_file[p->src1_reg].tag_value;
		}
		/* Rename Source Operand2 */
		if (register_file[p->src2_reg].ready == READY) {
			p->src2_ready = READY;
			p->src2_val = register_file[p->src2_reg].tag_value;
		} else if (register_file[p->src2_reg].ready == NOT_READY){
			p->src2_ready = NOT_READY;
			p->src2_val = register_file[p->src2_reg].tag_value;
		}
		/* Rename Destination Operand */
		register_file[p->dest_reg].ready = NOT_READY;
		register_file[p->dest_reg].tag_value = p->tag;

		i += 1;
	}

	/* For instructions in the dispatch_list that are in the IF stage, unconditionally transition to ID stage.
	   This models the one-cycle latency of the IF stage.
	*/
	p = dispatch_list->next;
	while (p != dispatch_list) {
		if (p->pipeline_stage == IF) {
			p->pipeline_stage = ID;
			tmp = fake_rob->next;
			while (tmp != fake_rob) {
				if (tmp->tag == p->tag) {
					tmp->pipeline_stage = ID;
					p = p->next;
					continue;
				}
				tmp = tmp->next;
			}
		}
		p = p->next;
	}

}

void issue()
{
	int i=0, temp_count=0;

	node_t *temp_list=0;
	node_t *p=0, *q=0, *tmp=0;

	/* From the issue_list, construct a temporary list of instructions whose operands are ready.
	*/
	temp_list = (node_t *)malloc(sizeof(node_t) * issue_count);
	if (!temp_list) {
		printf("Memory allocation failed!\n");
		printf("Exiting...\n");
		exit(1);
	}

	for (i=0 ; i < issue_count ; i++) {
		init_node(&temp_list[i]);
	}

	tmp = issue_list->next;
	temp_count = 0;

	for (i=0 ; i < issue_count ; i++) {
		if (tmp->pipeline_stage == IS && tmp->src1_ready == READY && tmp->src2_ready == READY) {
			copy_node(&temp_list[temp_count], tmp);
			temp_count += 1;
		}
		tmp = tmp->next;
	}

	/* Sort the temp_list in ascending order of tags. */
	sort_list(temp_list, temp_count);

	i = 0;
	while (i < temp_count && execute_count < N) {
		/* Remove the instruction from issue_list. */
		p = issue_list->next;
		q = NULL;
		while (p != issue_list) {
			if (p->tag == temp_list[i].tag) {
				if (!q) {
					issue_list->next = p->next;
				} else {
					q->next = p->next;
				}
				break;
			}
			q = p;
			p = p->next;
		}

		/* Add the removed entry to execute_list. */
		tmp = execute_list;
		while (tmp->next != execute_list)
			tmp = tmp->next;
		tmp->next = p;
		p->next = execute_list;

		/* Reserve a Functional Unit. */
		execute_count += 1;
		/* Free a Scheduling Queue Entry. */
		issue_count -= 1;

		/* Change pipeline stage from ID to IS. */
		p->pipeline_stage = EX;
		tmp = fake_rob->next;
		while (tmp != fake_rob) {
			if (tmp->tag == p->tag) {
				tmp->pipeline_stage = EX;
				break;
			}
			tmp = tmp->next;
		}

		i += 1;
	}
}

void print_fake_rob()
{
	node_t *p;

	printf("Fake Rob:->\n");
	p = fake_rob;
	while (p->next != fake_rob) {
		p = p->next;
		printf("%x %d %d %d %d %d %d\n", p->pc, p->op, p->dest_reg, p->src1_reg, p->src2_reg, p->pipeline_stage, p->tag);
	}
	printf("\n");
}

void print_dispatch_list()
{
	node_t *p;

	printf("Dispatch List:->\n");
	p = dispatch_list;
	while (p->next != dispatch_list) {
		p = p->next;
		printf("%x %d %d %d %d %d %d\n", p->pc, p->op, p->dest_reg, p->src1_reg, p->src2_reg, p->pipeline_stage, p->tag);
	}
	printf("\n");

}

void print_issue_list()
{
	node_t *p;

	printf("Issue List:->\n");
	p = issue_list;
	while (p->next != issue_list) {
		p = p->next;
		printf("%x %d %d %d %d %d %d\n", p->pc, p->op, p->dest_reg, p->src1_reg, p->src2_reg, p->pipeline_stage, p->tag);
	}
	printf("\n");

}

void print_execute_list()
{
	node_t *p;

	printf("Execute List:->\n");
	p = execute_list;
	while (p->next != execute_list) {
		p = p->next;
		printf("%x %d %d %d %d %d %d\n", p->pc, p->op, p->dest_reg, p->src1_reg, p->src2_reg, p->pipeline_stage, p->tag);
	}
	printf("\n");

}

