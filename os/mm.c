/*
 * mm.c
 *
 * This file contains code for memory management
 * for the yalnix operating system used by
 * the kernel
 *
 * Juan Suarez, Parth Dhanotra
 * Feb 7, 2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "ykernel.h"
#include "ylib.h"

// vm_enabled is an int value that indicates whether
// virtual memory is enabled or not. 1 means that its
// enabled and 0 means that its not enabled
int vm_enabled = 0;

// TOTAL_NUM_PAGES is the number of pages available
// calculated during run time
int TOTAL_NUM_PAGES;

// initialize_pagetable creates a block in memory of 
// contiguous pte_t which should function as a pagetable
pte_t* initialize_pagetable(int size) {
	ENTER;
	pte_t* pagetable = (pte_t*)calloc(size, sizeof(pte_t));
	if (pagetable == NULL) {
		TracePrintf(1, "malloc error: pagetable not created\n");
		exit(-1);
	}

	memset(pagetable, 0, size * sizeof(pte_t));

	return pagetable;

	EXIT;
}

// set_specific_pagetable_entry sets a specific pagetable entry
void set_specific_pagetable_entry(pte_t* pagetable, int index, unsigned int frame_number, unsigned int prot_mask) {
	ENTER;
	if (pagetable == NULL) {
		TracePrintf(1, "pointer error: pagetable not created\n");
		exit(-1);
	}

	if (index > MAX_PT_LEN) {
		TracePrintf(1, "index error: pagetable index beyond max pagetable size (MAX_PT_LEN)\n");
		exit(-1);
	}

	pagetable[index].pfn = frame_number;
	pagetable[index].prot = prot_mask;
	pagetable[index].valid = 1;

	return;

	EXIT;
} 

//disable_specific_pagetable_entry disables a specific pagetable entry
void disable_specific_pagetable_entry(pte_t* pagetable, int index) {
	ENTER;
	if (pagetable == NULL) {
		TracePrintf(1, "pointer error: pagetable not created\n");
		exit(-1);
	}

	if (index > MAX_PT_LEN) {
		TracePrintf(1, "index error: pagetable index beyond max pagetable size (MAX_PT_LEN)\n");
		exit(-1);
	}

	pagetable[index].valid = 0;

	return; 

	EXIT;
}

// frame_pool is an array of contiguous integers in
// memory where every integer corresponds to a frame
// in physical memory. If the integer is 1, the frame is
// being used, if its value is 0, then the frame is
// free
typedef struct {
	int* frame_pool_array;
	int frame_pool_size;
} frame_pool;

// initialize_frame_pool initializes the frame pool
// and returns a pointer to it
frame_pool* initialize_frame_pool(int size) {
	ENTER;
	frame_pool* frame_pool_created = (frame_pool*)malloc(sizeof(frame_pool));
	if (frame_pool_created == NULL) {
		TracePrintf(1, "malloc error: Frame pool not created\n");
		exit(-1);
	}	

	frame_pool_created->frame_pool_array = (int*)calloc(size, sizeof(int));
	if (frame_pool_created->frame_pool_array == NULL) {
		TracePrintf(1, "malloc error: Frame pool array not created\n");
		exit(-1);
	}

	frame_pool_created->frame_pool_size = size;

	memset(frame_pool_created->frame_pool_array, 0, size * sizeof(int));

	return frame_pool_created;
	EXIT;
}

// is_frame_used returns whether the frame at index is being used 
// or not. It returns -1 when there's an error
int is_frame_used(frame_pool* current_frame_pool, int index) {
	ENTER;
	if (current_frame_pool == NULL) {
		TracePrintf(1, "pointer error: frame pool pointer passed is null\n");
		exit(-1);
	}

	if (current_frame_pool->frame_pool_size <= index) {
		TracePrintf(1, "index error: frame pool index out of range\n");
		exit(-1);
	};

	return current_frame_pool->frame_pool_array[index];
	EXIT;
}

// set_frame sets the frame in the index to the value specified in the
// value parameter. Should be a value between zero and one.
void set_frame(frame_pool* current_frame_pool, int index, int value) {
	ENTER;
	if (current_frame_pool == NULL) {
		TracePrintf(1, "pointer error: frame pool pointer passed is null\n");
		exit(-1);
	}

	if (current_frame_pool->frame_pool_size <= index) {
		TracePrintf(1, "index error: frame pool index out of range\n");
		exit(-1);
	};

	current_frame_pool->frame_pool_array[index] = value;
	return;
	EXIT;
}

// get_unused_frame returns the index of an unused frame
// it returns -1 if no unused frame was found
int get_unused_frame(frame_pool* current_frame_pool) {
	ENTER;
	if (current_frame_pool == NULL) {
		TracePrintf(1, "pointer error: frame pool pointer passed is null\n");
		exit(-1);
	}

	for (int i = 0; i < current_frame_pool->frame_pool_size; i++) {
		if (current_frame_pool->frame_pool_array[i] == 0) {
			return i;
			EXIT;
		}
	}

	return -1;
	EXIT;
}

// destroy_frame_pool destroys the frame pool
void destroy_frame_pool(frame_pool* current_frame_pool) {
	ENTER;
	if (current_frame_pool == NULL) {
		TracePrintf(1, "pointer error: frame pool pointer passed is null\n");
		exit(-1);
	}

	free(current_frame_pool->frame_pool_array);
	free(current_frame_pool);
	return;
	EXIT;
}
