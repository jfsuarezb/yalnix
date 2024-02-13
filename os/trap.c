/*
 * trap.c
 *
 * This file contains the code for all
 * of the traps for the yalnix operating
 * system
 *
 * Juan Suarez, Parth Dhanotra
 *
 * Feb 7, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "ykernel.h"
#include "ylib.h"

// trap_clock is the TRAP_CLOCK trap
void trap_clock(UserContext* user_context) {

}

// trap_kernel is the TRAP_KERNEL trap
void trap_kernel(UserContext* user_context) {

}

// trap_illegal is the TRAP_ILLEGAL trap
void trap_illegal(UserContext* user_context) {

}

// trap_memory is the TRAP_MEMORY trap
void trap_memory(UserContext* user_context) {

}

// trap_math is the TRAP_MATH trap
void trap_math(UserContext* user_context) {

}

// trap_tty_receive is the TRAP_TTY_RECEIVE trap
void trap_tty_receive(UserContext* user_context) {

}

// trap_tty_transmit is the TRAP_TTY_TRANSMIT trap
void trap_tty_transmit(UserContext* user_context) {

}

// trap_disk is the TRAP_DISK trap
void trap_disk(UserContext* user_context) {

}

// trap_none is a trap that does nothing
void trap_none(UserContext* user_context) {

}
