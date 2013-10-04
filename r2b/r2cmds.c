#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

void cmd_mkproc(char* s, ProcessRecord* queue) {
  char namebuf[16] = "";
  char typebuf[16] = "";
  char priobuf[16] = "";
  int i, j;
  int failure = 0;
  ProcessControlBlock* newpcb;
  if (isspace(s[0]) ||
      (s[0] == '\0')) {
    printf("Must specify process details\n");
    failure = 1;
    goto fail;
  };
  for (i = 0; i < 15; i++) {
    if (i > 8) {
      printf("Name too long\n");
      failure = 1;
      goto fail;
    };
    if (isspace(s[i])) break;
    namebuf[i] = s[i];
  };
  namebuf[i] = '\0';
  for (i++, j = 0; j < 15; i++, j++) {
    if (j > 2) {
      printf("Invalid process type\n");
      failure = 1;
      goto fail;
    };
    if (isspace(s[i])) break;
    typebuf[j] = s[i];
  };
  typebuf[j] = '\0';
  if (isspace(s[i+1])) {
    printf("Must specify priority\n");
    failure = 1;
    goto fail;
  };
  for (i++, j = 0; j < 15; i++, j++) {
    if (j > 5) {
      printf("Invalid priority specified\n");
      failure = 1;
      goto fail;
    };
    if (isspace(s[i])) break;
    priobuf[j] = s[i];
  };
  priobuf[j] = '\0';
  switch (typebuf[0]) {
  case 's': 
  case 'S': i = SYS;
    break;
  case 'a':
  case 'A': i = APP;
    break;
  default:
    printf("Invalid process type\n");
    failure = 1;
    goto fail;
  };
  j = atoi(priobuf);
  if ((j < -128) ||
      (j > 127)) {
    printf("Priority out of range (-128:127)\n");
    failure = 1;
    goto fail;
  };
  newpcb = allocate_pcb();
  
  switch(setup_pcb(newpcb, namebuf, i)) {
  case SETUP_100:
    printf("Could not set process variables\n");
    failure = 1;
    goto fail;
  case SETUP_101:
    printf("Could not set process name\n");
    failure = 1;
    goto fail;
  case SETUP_110:
    printf("Could not set process type\n");
    failure = 1;
    goto fail;
  case SETUP_0XX:
    printf("Could not initialize process\n");
    failure = 1;
    goto fail;
  case 0:
    prioritize(newpcb, j);
    printf("Process data initialized:\n");
    printf("ID:       \t%s\nCLASS:    \t%d\nPRIORITY:\t%d\n", 
	   newpcb->name, newpcb->class, newpcb->priority);
    
  };
 fail:
  if (failure) printf("Error creating process\n");
};
void cmd_rmproc(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {/*
  remove_pcb(queue, s);
										*/
};

void cmd_block(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {
  ProcessControlBlock* target; /*
  target = find_pcb(name);
  if (target) {
    target->state = BLOCK;
    insert_pcb(target, 0);
    remove_pcb(target);
    }	*/
};
void cmd_unblock(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {/*
  ProcessControlBlock* target;
  target = find_pcb(name);
  if (target) {
    target->state = READY;
    insert_pcb(target, 1);
    remove_pcb(target);
    }*/
};
void cmd_setp(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {
  ProcessControlBlock* target;/*
  target = find_pcb(name);
  if (target) {
    target->priority = newPriority;
  }	
			      */
};
void cmd_suspend(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {/*
  ProcessControlBlock* target;
  target = find_pcb(name);
  if (target) {
    target->isSuspended = 1;
  }
										 */
};
void cmd_resume(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {/*
  ProcessControlBlock* target;
  target = find_pcb(name);
  if (target) {
    target->isSuspended = 0;
    }*/	
};
void cmd_showpcb(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {
};
void cmd_showall(char* s, QueueDescriptor* rdyQueue, QueueDescriptor* blkQueue) {
};
void cmd_showrdy(char* s, QueueDescriptor* queue) {
};
void cmd_showblk(char* s, QueueDescriptor* queue) {
};
