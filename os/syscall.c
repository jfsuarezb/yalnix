/*
 * syscall.c
 *
 * This file contains the system calls
 * for the yalnix os
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

// Fork system call
int Fork(void) {

}

// Exec system call
int Exec(char* filename, char** argvec) {

}

// Exit system call
void Exit(int status) {

}

// Wait system call
int Wait(int* status_ptr) {

}

// GetPid systemm call
int GetPid(void) {

}

// Brk system call
int Brk(void* addr) {

}

// Delay system call
int Delay(int clock_ticks) {

}


// TtyRead system call
int TtyRead(int tty_id, void* buf, int len) {

}

// TtyWrite system call
int TtyWrite(int tty_id, void* buf, int len) {

}

// PipeInit system call
int PIpeInit(int* pipe_idp) {

}

// PipeRead system call
int PipeRead(int pipe_id, void* buf, int len) {

}

// PipeWrite system call
int PipeWrite(int pipe_id, void* buf, int len) {

}

// LockInit system call
int LockInit(int* lock_idp) {

}

// Acquire system call
int Acquire(int lock_id) {

}

// Release system call
int Release(int lock_id) {

}

// CvarInit system call
int CvarInit(int* cvar_idp) {

}

// CvarSignal system call
int CvarSignal(int cvar_id) {

}

// CvarBroadcast system call
int CvarBroadcast(int cvar_id) {

}

// CvarWait system call
int CvarWait(int cvar_id, int lock_id) {

}

// Reclaim system call
int Reclaim(int id) {

}
