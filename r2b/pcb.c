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

void queue_init() {
READYQ = sys_alloc_mem(sizeof(ProcessRecord));
BLOCKQ = sys_alloc_mem(sizeof(ProcessRecord));
RDESC = sys_alloc_mem(sizeof(QueueDescriptor));
BDESC = sys_alloc_mem(sizeof(QueueDescriptor));
}
void insert_pcb(ProcessControlBlock *newpcb, int mode) {
 ProcessRecord *newPtr, *previousPtr, *currentPtr;
  newPtr = sys_alloc_mem(sizeof(ProcessRecord));
  
  if (newPtr != NULL) {
    newPtr = newpcb;
    newPtr->next = NULL;
	if(mode) {
	currentPtr = READYQ;
		while (currentPtr != NULL && currentPtr->pcb->priority > newpcb->priority) {
			previousPtr = currentPtr;
			currentPtr = currentPtr->next;
        }
		if (previousPtr == NULL) {
			newPtr->next = READYQ;
			READYQ = newPtr;
			RDESC->head = READYQ;
        } else {
			previousPtr->next = newPtr;
			newPtr->next = currentPtr;
			newPtr->prev = previousPtr;
			currentPtr->prev = newPtr;
			if(currentPtr == NULL){RDESC->tail = newPtr;}
        };
    } else {
      currentPtr = BLOCKQ;
	    while (currentPtr != NULL) {
			previousPtr = currentPtr;
			currentPtr = currentPtr->next;
        }
        if (previousPtr == NULL) {
			newPtr->next = BLOCKQ;
			BLOCKQ = newPtr;
			BDESC->head = BLOCKQ;
        } else {
			previousPtr->next = newPtr;
			newPtr->next = currentPtr;
			newPtr->prev = previousPtr;
			if(currentPtr == NULL){BDESC->tail = newPtr;}
        };
	};
  } else {
      printf("Process not inserted. No memory available.\n");
   };
}
 
ProcessRecord* find_pcb(char* name) {
ProcessRecord *tempPtr;
int found = 0;
int searchedblock = 0;
tempPtr = RDESC->head;
 
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
	if(tempPtr == NULL && !searchedblock) {
	tempPtr = BDESC->head;
	searchedblock = 1;
	}
}
if(!found){tempPtr = NULL;}
return tempPtr;
}

int remove_pcb(char* name) {
int freed;
ProcessRecord *target, *temp;
target = find_pcb(name);

if(target != NULL) {
temp = target->prev;
temp->next = target->next;

temp = target->next;
temp->prev = target->next;
sys_free_mem(target);
freed = 1;
} else {
printf("Failed to remove process\n");
freed = 0;
}
return freed;
}
