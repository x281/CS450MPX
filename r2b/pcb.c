#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

ProcessControlBlock* allocate_pcb() {
  ProcessControlBlock *data;
  int i;
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
  data->stackSpace = sys_alloc_mem(PCB_STACK_SIZE * sizeof(unsigned char));
  for(i = 0; i < PCB_STACK_SIZE; i++) {
    printf(".");
    data->stackSpace[i] = '0';
  };
  printf("\n");
  data->stackTop = &(data->stackSpace[PCB_STACK_SIZE]);
  return data;
};

int free_pcb(ProcessControlBlock* pcb) {

  if ((sys_free_mem(pcb->stackTop) == 0) &&
      (sys_free_mem(pcb->stackSpace) == 0) &&
      (sys_free_mem(pcb) == 0)) {
    return 0; 
  };
  return -1;
};

int setup_pcb(ProcessControlBlock* pcb, 
	      char* set_name,
	      proc_class set_class) {
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
      pcb->priority    = -128;
      pcb->state       = ready_s;
      pcb->isSuspended = 0;
      pcb->memorySize  = 0;
      pcb->load_addr   = NULL;
      pcb->exec_addr   = NULL;
    } else {
    };
  };
  return retVal;
}; 

int insert_pcb(ProcessControlBlock* ins, QueueDescriptor* queue, int pMode) {
  if (!pMode) {
    ProcessRecord *rec;
    ProcessRecord *tmp;
    
    rec = sys_alloc_mem(sizeof(ProcessControlBlock *) +
			sizeof(ProcessRecord *) +
			sizeof(ProcessRecord *));
      rec->pcb  = ins;
           tmp  = queue->head;
      tmp->prev = rec;
      rec->next = tmp;
    queue->head = rec;
  };
};
