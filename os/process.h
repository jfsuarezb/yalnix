/*
 * process.h
 *
 * This file is the header file
 * for the data structures and 
 * operations that manage processes in
 * yalnix
 *
 * Juan Suarez, Parth Dhanotra
 *
 * Feb 7, 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "ykernel.h"
#include "mm.h"
#include "ylib.h"


// PCB is the process control block structure
typedef struct pcb {
	int pid;
	pagetable* pagetable_ptr;
	UserContext* UserContext_ptr;
	KernelContext* KernelContext_ptr;
	PCB* parent_process;
        PCB* child_processes[];
	int num_child_processes;	
	unsigned int first_addr;
	unsigned int brk;
	unsigned int stack_pointer;
	unsigned int last_addr;
} PCB;
