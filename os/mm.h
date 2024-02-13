/*
 * mm.h
 *
 * This file is the header file for memory management
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

// global_virtual_memory_enabled is a variable that denotes
// whether the virtual memory has been enabled
extern int global_virtual_memory_enabled;

// TOTAL_NUM_PAGES is the number of pages available
// calculated during run time
extern int TOTAL_NUM_PAGES;

// initialize_pagetable creates a block in memory of 
// contiguous pte_t which should function as a pagetable
pte_t* initialize_pagetable(int size);

// set_specific_pagetable_entry sets a specific pagetable entry
void set_specific_pagetable_entry(pte_t* pagetable, int index, unsigned int frame_number, unsigned int prot_mask);

//disable_specific_pagetable_entry disables a specific pagetable entry
void disable_specific_pagetable_entry(pte_t* pagetable, int index);

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
frame_pool* initialize_frame_pool(int size);

// is_frame_used returns whether the frame at index is being used 
// or not. It returns -1 when there's an error
int is_frame_used(frame_pool* current_frame_pool, int index);

// set_frame sets the frame in the index to the value specified in the
// value parameter. Should be a value between zero and one.
void set_frame(frame_pool* current_frame_pool, int index, int value);

// get_unused_frame returns the index of an unused frame
// it returns -1 if no unused frame was found
int get_unused_frame(frame_pool* current_frame_pool);

// destroy_frame_pool destroys the frame pool
void destroy_frame_pool(frame_pool* current_frame_pool);
