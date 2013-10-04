#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

ProcessControlBlock* allocate_pcb() {
  ProcessControlBlock* result;
  int i;
  result = (ProcessControlBlock*)malloc((size_t)sizeof(ProcessControlBlock));
  for(i = 0; i < PCB_STACK_SIZE; i++) {
    result->stackSpace[i] = (unsigned char)'1';
    //    printf("%c", result->stackSpace[i]);
  };
  result->stackTop = &(result->stackSpace[PCB_STACK_SIZE]);
  //  printf("\n");
  return result;
};


int free_pcb(ProcessControlBlock* pcb) {
  
  if (//(sys_free_mem(pcb->stackTop) == 0) &&
      //(sys_free_mem(pcb->stackSpace) == 0) &&
      (free(pcb) == 0)) {
    return 0; 
  };
  return -1;
};

void prioritize(ProcessControlBlock* pcb, int p) {
  pcb->priority = p;
};

int setup_pcb(ProcessControlBlock* pcb, 
	      char* set_name,
	      int set_class) {
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
    if ((set_class != SYS ) &&
	(set_class != APP )) {
      retVal -= 7;
    } else {
      pcb->class = set_class;
    };
    if (retVal == 0) {
      //Finish initializing fields:
      pcb->priority    = -128;
      pcb->state       = READY;
      pcb->isSuspended = 0;
      pcb->memorySize  = 0;
      pcb->load_addr   = NULL;
      pcb->exec_addr   = NULL;
    } else {
    };
  };
  return retVal;
}; 

int create_pcb(char* name, int cls, int priority) {
  ProcessControlBlock *newpcb;
  newpcb = allocate_pcb();
  return setup_pcb(newpcb, name, cls);
};

ProcessRecord* queue_init() {
  ProcessRecord *readyq = sys_alloc_mem(sizeof(ProcessRecord));
  Linker *link = sys_alloc_mem(sizeof(Linker));
  readyq->next = link;
  link->prev = readyq;
  link->next = NULL;
  link->name = NULL;
  link->state = 0;
  return readyq;
};

void insert_pcb (ProcessRecord *queue, ProcessControlBlock *newpcb) {
  ProcessRecord *newPtr, *previousPtr, *currentPtr;
  newPtr = sys_alloc_mem(sizeof(ProcessRecord));
  
  if (newPtr != NULL) {
    newPtr = newpcb;
    newPtr->next = NULL;
    currentPtr = queue;
    if(newpcb->state == 2) {
      while ((currentPtr->pcb->state != 0) && 
	     (currentPtr != NULL) && 
	     (currentPtr->pcb->priority > newpcb->priority)) {
	previousPtr = currentPtr;
	currentPtr = currentPtr->next;
      }
      if (previousPtr->prev == NULL) {
	newPtr->next = queue;
	queue = newPtr;
      } else {
	previousPtr->next = newPtr;
	newPtr->next = currentPtr;
	newPtr->prev = previousPtr;
	currentPtr->prev = newPtr;
      };
    } else {
      
      while (currentPtr != NULL) {
	previousPtr = currentPtr;
	currentPtr = currentPtr->next;
      }
      if (previousPtr == NULL) {
	newPtr->next = queue;
	queue = newPtr;
      } else {
	previousPtr->next = newPtr;
	newPtr->next = currentPtr;
	newPtr->prev = previousPtr;
      };
    };
  } else {
    printf("Process not inserted. No memory available.\n");
  };
};

ProcessRecord* find_pcb(ProcessRecord* queue, char* name) {
  ProcessRecord *tempPtr;
  int found = 0;
  tempPtr = queue;
  
  while(tempPtr != NULL) {
    if(tempPtr->pcb->state) {
      if(strcmp(tempPtr->pcb->name,name)) {	
	found = 1;
	break;
      } else {
	tempPtr = tempPtr->next;
      }
    } else {
      tempPtr = tempPtr->next;
    }
  }
  if(!found){tempPtr = NULL;}
  return tempPtr;
}

int remove_pcb(ProcessRecord* queue, char* name) {
  int freed;
  ProcessRecord *target, *temp;
  target = find_pcb(queue,name);
  
  if(target != NULL) {
    temp = target->prev;
    temp->next = target->next;
    
    temp = target->next;
    temp->prev = target->next;
    free(target);
    freed = 1;
  } else {
    printf("Failed to remove process\n");
    freed = 0;
  }
  return freed;
}
