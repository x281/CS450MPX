#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

QueueDescriptor *READYQ;
QueueDescriptor *BLOCKQ;

void init_queues() {
  READYQ = sys_alloc_mem(sizeof(QueueDescriptor));
  BLOCKQ = sys_alloc_mem(sizeof(QueueDescriptor));
  READYQ->head = NULL;
  BLOCKQ->head = NULL;
  READYQ->tail = NULL; 
  BLOCKQ->tail = NULL; 
};

void dispose_queues() {
  ProcessRecord* tmp;
  tmp = READYQ->head;
  if (tmp) {
    do {
      remove_pcb(READYQ, tmp->pcb->name);
      if (free_pcb(tmp->pcb) != 0) {
	printf("Error dequeueing PCB\n");
      };
      sys_free_mem(tmp);
      tmp = READYQ->head;
    } while (tmp);
  };
  tmp = BLOCKQ->head;
  if (tmp) {
    do {
      remove_pcb(BLOCKQ, tmp->pcb->name);
      if (free_pcb(tmp->pcb) != 0) {
	printf("Error dequeueing PCB\n");
      };
      sys_free_mem(tmp);
      tmp = BLOCKQ->head;
    } while (tmp);
  };
};

ProcessControlBlock* allocate_pcb() {
  ProcessControlBlock* result;
  int i;
  result = (ProcessControlBlock*)sys_alloc_mem(sizeof(ProcessControlBlock));
  result->stackSpace = (unsigned char*)sys_alloc_mem(PCB_STACK_SIZE *
						     sizeof(unsigned char));
  for(i = 0; i < PCB_STACK_SIZE; i++) {
    result->stackSpace[i] = (unsigned char)'0';
  };
  result->stackTop = &(result->stackSpace[PCB_STACK_SIZE]);
  return result;
};


int free_pcb(ProcessControlBlock* pcb) {
  if ((sys_free_mem(pcb->stackSpace) == 0) &&
      (sys_free_mem(pcb) == 0)) {
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


void insert_pcb(ProcessControlBlock* ins, QueueDescriptor* dest, int pMode) {
  ProcessRecord* new;
  new = (ProcessRecord *)sys_alloc_mem(sizeof(ProcessRecord));
  if (new != NULL) {
    new->pcb = ins;
    if (dest->head == NULL) {
      // This PCB will comprise the whole queue:
      new->next = new;
      new->prev = new;
      dest->head = new;
      dest->tail = new;
    } else {// Queue not empty!
      if (pMode) {
	// Insert by priority:
	ProcessRecord* ptr = dest->head;
	while (ptr->pcb->priority > new->pcb->priority) {
	  ptr = ptr->next;
	  if (ptr == dest->tail) break;
	};

	if (ptr->pcb->priority > new->pcb->priority) {
	  // Insert after pointer, reassign tail:
	  ProcessRecord* tmp = ptr->next;
	  tmp->prev = new;
	  ptr->next = new;
	  new->next = dest->head;
	  dest->tail = new;
	  new->prev = ptr;
	} else {
	  ProcessRecord* tmp = ptr->prev;
	  if (ptr == dest->head) {
	    // move head:
	    dest->head = new;
	  };
	  // insert ahead of pointer:
	  new->next = ptr;
	  tmp->next = new;
	  new->prev = ptr->prev;
	  ptr->prev = new;
	};
      } else {
	// Insert at tail (FIFO mode!)
	ProcessRecord* ptr = dest->tail;
	ProcessRecord* tmp = ptr->next;
	tmp->prev = new;
	ptr->next = new;
	new->next = dest->head;
	dest->tail = new;
	new->prev = ptr;
      };
    };
  } else {
    printf("ERROR: Unable to allocate PCB record space\n");
  };
  printf("Inserted: %s\t%d\n"
	 "    Head: %s\t%d\n"
	 "    Tail: %s\t%d\n",
	 new->pcb->name, new->pcb->priority,
	 dest->head->pcb->name, dest->head->pcb->priority,
	 dest->tail->pcb->name, dest->tail->pcb->priority
	 );
};


ProcessRecord* find_pcb(char* name) {
  if (READYQ->head != NULL) {
    //Search the Ready Queue:
    ProcessRecord* ptr = READYQ->head;
    do {
      if (strcmp(ptr->pcb->name, name) == 0) return ptr;
      ptr = ptr->next;
    } while (ptr != READYQ->head);
  };
  if (BLOCKQ->head != NULL) {
    //Search the Blocked Queue:
    ProcessRecord* ptr = BLOCKQ->head;    
    do {
      if (strcmp(ptr->pcb->name, name) == 0) return ptr;
      ptr = ptr->next;
    } while (ptr != BLOCKQ->head);
  };
  return NULL;
};


int remove_pcb(QueueDescriptor* queue, char* name) {
  int freed;
  ProcessRecord *target, *temp;
  target = find_pcb(name);
  if (target == NULL) return -1;

  if ((target == queue->head) &&
      (target == queue->tail)) {
    printf("Nullifying queue head and tail\n");
    queue->head = NULL;
    queue->tail = NULL;
    target->next = NULL;
    target->prev = NULL;
    return 0;
  };

  temp = queue->head;

  do {
    if (temp == target) break;
    temp = temp->next;
  } while (temp != queue->head);

  if (temp != target) return -2;

  temp = target->prev;
  temp->next = target->next;
  if (target == queue->tail) queue->tail = temp;
  temp = target->next;
  temp->prev = target->prev;
  if (target == queue->head) queue->head = temp;

  return 0;
}
