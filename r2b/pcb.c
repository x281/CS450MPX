#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

ProcessControlBlock* allocate_pcb() {
  ProcessControlBlock *data;
  data = sys_alloc_mem((9 * sizeof(char)) +
		       (sizeof(proc_class)) +
		       (sizeof(int)) +
		       (sizeof(proc_state)) + 
		       (sizeof(int)) +
		       (sizeof(unsigned char *)) +
		       (sizeof(unsigned char *)) +
		       (sizeof(int)) +
		       (sizeof(unsigned char *)) +
		       (sizeof(unsigned char *)));
  data->stackSpace = sys_alloc_mem(1024 * sizeof(unsigned char));
  data->stackTop   = data->stackSpace[sizeof(data->stackSpace)];
  return data;
};

int free_pcb(ProcessControlBlock* pcb) {
  if (sys_free_mem(pcb) == 0) {
    return 0; }
  else {
    return -1;
  };
};

int setup_pcb(ProcessControlBlock* pcb, 
	      char* set_name,
	      proc_class set_class) {
  int pcbSet, nameSet, classSet;
  int retVal;
  retVal = 0;
  /*Return SETUP_110, SETUP_101, 
    SETUP_100, or SETUP_0XX based
    on various field init status */
  if (!pcb) {
    retVal -= 3;
  } else {
    if (!set_name) {
      retVal -= 5;
    } else {
      strcpy(pcb->name, set_name);
    };
    if ((set_class != sys_proc) &&
	(set_class != pgm_proc)) {
      retVal -= 7;
    } else {
      pcb->class = set_class;
    };
    if (retVal == 0) {
      //Finish initializing fields:
      pcb->priority = -128;
      pcb->state = ready_s;
      pcb->isSuspended = 0;
      pcb->memorySize = 0;
      pcb->load_addr = NULL;
      pcb->exec_addr = NULL;
    } else {
    };
  };
  return retVal;
}; 
