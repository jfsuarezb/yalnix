/*
 * kernel.c
 *
 * This file contains the kernel start
 * function along with other data structures
 * and operations that boot and run
 * the yalnix os
 *
 * Juan Suarez, Parth Dhanotra
 * Feb 7, 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "ykernel.h"
#include "mm.h"
#include "process.h"
#include "yalnix.h"
#include "ylib.h"
#include "trap.h"

// global_frame_pool is the global frame pool that tracks which frames are free and which are being used
frame_pool* global_frame_pool;

// global_kernel_pagetable is the kernel pagetable
pte_t* global_kernel_pagetable;

// global_kernel_brk is the kernel bkr address
unsigned int global_kernel_brk = _orig_kernel_brk_page << PAGESHIFT;

// global_trap_vector is an array of function pointes that will be used
// as the vector for trap routines
void (*global_trap_vector[TRAP_VECTOR_SIZE])(UserContext*);

// SetKernelBrk is a system call that the kernel uses to expand the heap to addr
int SetKernelBrk(void* addr) {
	if (!global_virtual_memory_enabled) {
		global_kernel_brk = (unsigned int)addr;
		return 0;
	} else {
		int current_base = DOWN_TO_PAGE(global_kernel_brk);
		int new_limit = DOWN_TO_PAGE((unsigned int)addr);
		
		for (int i = current_base, i <= new_limit; i++) {
			int new_frame = get_unused_frame(global_frame_pool);			
			if (new_frame == -1) {
				TracePrintf(1, "no new frame found\n");
				return ERROR;
			}

			set_frame(global_frame_pool, new_frame, 1);

			set_specific_pagetable_entry(global_kernel_pagetable, i, new_frame, PROT_READ|PROT_WRITE);
		}

		global_kernel_brk = (unsigned int)addr;
		return 0;
	}
}

// DoIdle function
void DoIdle(void) {
	while(1) {
		TracePrintf(1, "DoIdle\n");
		Pause();
	}
}

// KernelStart is the function that boots the kernel
// and begins running the yalnix os
void KernelStart(char* cmd_args[], unsigned int pmem_size, UserContext* uctxt) {
	// ENTER trace
	ENTER;

	// Calculating number of pages
	TOTAL_NUM_PAGES = pmem_size / PAGESIZE;
		
	// Setting up frame pool tracker	
	global_frame_pool = initialize_frame_pool(TOTAL_NUM_PAGES);

	// initializing the global kernel pagetable
	global_kernel_pagetable = initialize_pagetable(MAX_PT_LEN);

	// setting kernel brk
	global_kernel_brk = _orig_kernel_brk_page;

	// setting kernel pagetables as used in frame pool
	for (int i = _first_kernel_text_page; i < _orig_kernel_brk_page; i++) {
		set_frame(global_frame_pool, i, 1);
	}	

	// allocating kernel page tables 
	for (int i = _first_kernel_text_page; i < _first_kernel_data_page; i++) {
		set_specific_pagetable_entry(global_kernel_pagetable, i, i, PROT_READ|PROT_EXEC); 	
	}

	for (int i = _first_kernel_data_page; i < _orig_kernel_brk_page; i++) {
		set_specific_pagetable_entry(global_kernel_pagetable, i, i, PROT_READ|PROT_WRITE); 	
	}

	// setting kernel stack pagetables as used in frame pool
	set_frame(global_frame_pool, KERNEL_STACK_BASE, 1);
	set_frame(global_frame_pool, KERNEL_STACK_BASE + 1, 1);	

	// allocating kernel stack page tables
	set_specific_pagetable_entry(global_kernel_pagetable, KERNEL_STACK_BASE, KERNEL_STACK_BASE, PROT_READ|PROT_WRITE);
	set_specific_pagetable_entry(global_kernel_pagetable, KERNEL_STACK_BASE + 1, KERNEL_STACK_BASE + 1, PROT_READ|PROT_WRITE);

	// writing MMU registers for Region 0 (kernel) memory
	WriteRegister(REG_PTBR0, (unsigned int)global_kernel_pagetable);
	WriteRegister(REG_PTLR0, VMEM_0_SIZE);

	// initializing region 1 pagetable
	pte_t* user_pagetable = initialize_pagetable(MAX_PT_LEN);
	
	// getting the user stack virtual pagetable number
	int user_stack_pagetable_num = DOWN_TO_PAGE(VMEM_1_LIMIT - (2 * PAGESIZE));

	// getting two unused pagetables for the user stack
	int u_stack_f1 = get_unused_frame(global_frame_pool);
	set_frame(global_frame_pool, u_stack_f1, 1);	

	int u_stack_f2 = get_unused_frame(global_frame_pool);
	set_frame(global_frame_pool, u_stack_f2, 1);	
	
	// allocating usetr stack pagetables
	set_specific_pagetable_entry(user_pagetable, user_stack_pagetable_num, u_stack_f1, PROT_READ|PROT_WRITE);
	set_specific_pagetable_entry(user_pagetable, user_stack_pagetable_num + 1, u_stack_f2, PROT_READ|PROT_WRITE);

	// writing MMU registers for Region 1 (user) memory
	WriteRegister(REG_PTBR1, (unsigned int)user_pagetable);
	WriteRegister(REG_PTLR0, VMEM_1_SIZE);

	// checking if heap increased in size before enabling virtual memory
	if ((_orig_kernel_brk_page << PAGESHIFT) < global_kernel_brk) {
		int current_base = DOWN_TO_PAGE(_orig_kernel_brk_page << PAGESHIFT);
		int new_limit = DOWN_TO_PAGE(global_kernel_brk);

		for (int i = current_base, i <= new_limit; i++) {
			int new_frame = get_unused_frame(global_frame_pool);			
			if (new_frame == -1) {
				TracePrintf(1, "no new frame found\n");
				return ERROR;
			}

			set_frame(global_frame_pool, new_frame, 1);

			set_specific_pagetable_entry(global_kernel_pagetable, i, new_frame, PROT_READ|PROT_WRITE);
		}
	}

	// Setting Trap Vector table
	global_trap_vector[0] = trap_kernel;
	global_trap_vector[1] = trap_clock;
	global_trap_vector[2] = trap_illegal;
	global_trap_vector[3] = trap_memory;
	global_trap_vector[4] = trap_math;
	global_trap_vector[5] = trap_tty_recieve;
	global_trap_vector[6] = trap_tty_transmit;
	global_trap_vector[7] = trap_disk;
	for (int i = 8; i < TRAP_VECTOR_SIZE; i++) {
		global_trap_vector[i] = trap_none;
	}
	int base_trap_vector_address = (int)(&global_trap_vector[0]);
	WriteRegister(REG_VECTOR_BASE, base_trap_vector_address);

	// enabling virtual memory
	global_virtual_memory_enabled = 1;
	WriteRegister(REG_VM_ENABLE, 1);

	// running DoIdle
	DoIdle();

	return 0;
	EXIT;
}
