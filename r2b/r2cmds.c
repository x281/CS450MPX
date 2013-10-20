#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

void cmd_mkproc(char* s) {
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
    insert_pcb(newpcb, READYQ, 1);
    
  };
 fail:
  if (failure) printf("Error creating process\n");
};

void cmd_rmproc(char* s) {
  ProcessRecord* tgt;
  s[strlen(s)-1] = '\0';
  tgt = find_pcb(s);

  if ((remove_pcb(READYQ, s) == 0) ||
      (remove_pcb(BLOCKQ, s) == 0)) {
    free_pcb(tgt->pcb);
    sys_free_mem(tgt);
  };
};

void cmd_block(char* s) {
  ProcessRecord* target; 
  s[strlen(s)-1] = '\0';
  target = find_pcb(s);
  if (target) {
    target->pcb->state = BLOCKED;
    if (remove_pcb(READYQ, s) == 0) {
      insert_pcb(target->pcb, BLOCKQ, 0);
      printf("Process %s blocked\n", s);
    } else {
      printf("Error dequeueing process.\n");
    };
  } else {
    printf("Process %s not found!\n", s);
  };
};

void cmd_unblock(char* s) {
  ProcessRecord* target;
  s[strlen(s)-1] = '\0';
  target = find_pcb(s);
  if (target) {
    target->pcb->state = READY;
    if (remove_pcb(BLOCKQ, s) == 0) {
      insert_pcb(target->pcb, READYQ, 1);
      printf("Process %s unblocked\n", s);
    } else {
      printf("Error dequeueing process.\n");
    };
  } else {
    printf("Process %s not found!\n", s);
  };
};

void cmd_setp(char* s) {
  ProcessRecord* target;
  char pBuffer[5];
  int i = 0;
  int j;
  while (!(isspace(s[i]))) {
    i++;
  };
  s[i++] = '\0';
  target = find_pcb(s);
  if (target) {
    for(j = i, i = 0; i < 5; i++, j++) {
      pBuffer[i] = s[j];
      if (isspace(s[j])) break;
    };
    if (i < 5) { 
      pBuffer[i] = '\0';
      j = atoi(pBuffer);
      if ((j < -128) || (j > 127)) goto errlen;
      target = find_pcb(s);
      if (target) {
	target->pcb->priority = j;
      };
    } else {
    errlen:
      printf("Priority out of range (-128:127)\n");
    };
  } else {
    printf("Target PCB not found\n");
  };
};

void cmd_suspend(char* s) {
  ProcessRecord* target; /*
  target = find_pcb(name);
  if (target) {
    target->isSuspended = 1;
  }
										 */
};

void cmd_resume(char* s) {
  ProcessControlBlock* target; /*
  target = find_pcb(name);
  if (target) {
    target->isSuspended = 0;
    }*/	
};

void cmd_showpcb(char* s) {
  ProcessRecord* tmp;
  s[strlen(s)-1] = '\0';
  tmp  = find_pcb(s);
  printf("Searching for %s\n", s);
  if (tmp != NULL) {
    printf("PCB Name: %s\n"
	   "   Class: %c\n"
	   "Priority: %d\n"
	   "   State: %c\n", 
	   tmp->pcb->name,
	   (tmp->pcb->class == 1) ? 'S' : 'A',
	   tmp->pcb->priority,
	   (tmp->pcb->state == 1) ? '\*' :
	   ((tmp->pcb->state == 2) ? 'R' : 'B'));
  };
};

void cmd_showall(char* s) {
};

void cmd_showrdy(char* s) {
  ProcessRecord* tmp;
  tmp = READYQ->head;
  if (tmp == NULL) {
    printf("Ready queue empty!\n");
    goto end;
  };
  do {
    printf("PCB Name: %s\n"
	   "   Class: %c\n"
	   "Priority: %d\n", 
	   tmp->pcb->name,
	   (tmp->pcb->class == 1) ? 'S' : 'A',
	   tmp->pcb->priority);
    tmp = tmp->next;
 } while (tmp != READYQ->head);
  end:
};

void cmd_showblk(char* s) {
};
