#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"

/****************************

  CS 450 MPX Operating System
  Module R2

  TEAM TEAMWORK
  Erik Bernard
  James Epting
  Chris Ho
  Darren Ringer
  
  **************************/


//Possible command index
#define CMD_IDX \
\
  cmdArray['c'-'a'][0] = "clear";\
  cmdArray['d'-'a'][0] = "date";\
  cmdArray['d'-'a'][1] = "dir";\
  cmdArray['h'-'a'][0] = "help";\
  cmdArray['m'-'a'][0] = "mpxdir";\
  cmdArray['q'-'a'][0] = "quit";\
  cmdArray['r'-'a'][0] = "random";\
  cmdArray['s'-'a'][0] = "setdate";\
  cmdArray['t'-'a'][0] = "test";\
  cmdArray['v'-'a'][0] = "version";



//Function Prototypes
/* comhan.c */
int   mpx_command_loop();

/* commands.c */
void  cmd_mpxdir(char*);
void  cmd_version();
void  cmd_help();
int   cmd_set_date(date_rec*, char*);
void  cmd_disp_date(date_rec*);
void  cmd_clear();

/* prng.c */
long  rnd();
int   randi(int);
float randf();
void  rand_init();

/* pcb.c */
ProcessControlBlock* allocate_pcb();
                int  free_pcb(ProcessControlBlock*);
                int  setup_pcb(ProcessControlBlock*, 
			       char*, proc_class);
ProcessControlBlock* find_pcb(char*);
                int  insert_pcb(ProcessControlBlock, QueueDescriptor, int);
                int  remove_pcb(ProcessControlBlock*, QueueDescriptor);



///////////////////
/* MAIN FUNCTION */
///////////////////
int main() {
  int ini;

  QueueDescriptor readyQueue;
  QueueDescriptor blockedQueue;

  rand_init();
  
  //Init sys helper
  ini = sys_init(MODULE_R1);

  //Start cmd loop
  if (ini == 0) {

    mpx_command_loop();

    //Shutdown sys helper
    sys_exit();
  } else printf("SYSTEM ERROR: Unable to initialize library");

  printf("[MPX System Shutdown]\n");

  return 0;
};
///////////////////
/*\END\\END\\END\*/
///////////////////




/////////////////
//Command loop //
/////////////////
int mpx_command_loop() {
  int ck;

  char entry[128];    //user input
  long sz;            //length
  char cmd[CMDSIZE+1];//1st word
  
  date_rec *day;      //date

  char ***cmdArray;   //table of cmds

  char c;   //  generic //
  int i, j; //variables //

  char pa[3] = "[]";//re:cmd//
  char pb[3] = "()";//prompt//

  /* Initialize memory */
  cmdArray = sys_alloc_mem(26 * sizeof(char **));
  for (i = 0; i < 26; i++) {
    cmdArray[i] = sys_alloc_mem(CMDNUM * sizeof(char *));
    for (j = 0; j < CMDNUM; j++) {
      cmdArray[i][j] = sys_alloc_mem((CMDSIZE + 1) * sizeof(char));
    };
  };
  
  cmd_clear();
  printf("[MPX System Initialized]\n");
  cmd_version();
  cmd_disp_date(day);

  /* Possible command index */
  CMD_IDX  
  
  /* Begin Command Loop */
  ck = 0;
 cloop:
  while(ck >= 0) {
    int spaceCount; // count of spaces at front

    /*MAIN COMMAND PROMPT*/
    printf("%c%c> ", pa[randi(2)], pb[randi(2)]);
    ck = sys_req(READ, TERMINAL, entry, &sz);
    if (ck < 0) {
      printf("FATAL ERROR\n");
      goto stop;
    };

    /*Skip leading whitespace*/
    spaceCount = 0;
    if (entry[0] != '\n') {
      for(i = 0; i < sz; i++) {
	if (isspace(entry[i])) {spaceCount++;}
	else break;
      };
    };

    /* Put first word of entry in cmd buffer */
    for(i = spaceCount; i < sz; i++) {
      if (i > (spaceCount+CMDSIZE)) {
	printf("Command not found: too many characters\n");
	entry[0] = '\0';//reset buffer
	goto broke;
      };
      c = entry[i];
      if (isspace(c)) {
	cmd[i - spaceCount] = '\0';
	break;}
      else {
	cmd[i - spaceCount] = c;};};
    
    /* Scan for the command in cmdArray */
    for(i = 0; i < CMDNUM; i++) {
      int hit;
      for(j = 0; j < (CMDSIZE + 1); j++) { 
	hit = 0;
	if ((cmd[j] != '\0') && (cmd[j] < 'a')) cmd[j] += 32;
	if (cmd[j] != cmdArray[cmd[0]-'a'][i][j]) break;
	hit = 1;
       	if (cmd[j] == '\0') break;
      };
      if (hit == 0) {
	if ((i+1) == CMDNUM) {
	  printf("Command not found: unknown\n"); 
	  break; }
	else goto retry;
      };


      /* Assuming cmd was found, handle appropriately */
      switch (i) {
      case 0: {
	switch(cmd[0]) {
   // DATE:
	case 'd': cmd_disp_date(day);
	  break;
   // HELP:
	case 'h': cmd_help();
	  break;
   // MPXDIR:
	case 'm': cmd_mpxdir(&entry[j+1+spaceCount]);
	  break;
   // QUIT:
	case 'q': goto stop;
 
   // RANDOM:
	case 'r': printf("%d\n", randi(atoi(&entry[j+1+spaceCount])));
	  break;
   // SETDATE:
	case 's': cmd_set_date(day, &entry[j+1+spaceCount]);
	  break;
   /* TEST COMMAND */
	case 't': {
	  ProcessControlBlock *test_pcb;
	  char *name = "test pcb";
	  proc_class class = pgm_proc;
	  test_pcb = allocate_pcb();
 	  printf("TEST\n%d\n",
		 setup_pcb(test_pcb, name, class));
	  free_pcb(test_pcb);
	  break;
	};
   // VERSION:
	case 'v': cmd_version();
	  break;
   // CLEAR:
	case 'c': cmd_clear();
	  break;
	};
	break;
      };
      case 1: {
	switch(cmd[0]) {
   // DIR:  
	case 'd': cmd_mpxdir(&entry[j+1+spaceCount]);
	  break;
	};
      };
      };
      /* Command was found but not
	 handled here?: */
      if (hit == 1) break;
    retry:
    };
    
  broke:
  };
  
  /* Deallocate memory/shutdown */
 stop:
  for (i = 0; i < 26; i++) {
    for (j = 0; j < CMDNUM; j++) {
      sys_free_mem(cmdArray[i][j]);
    };
    sys_free_mem(cmdArray[i]);
  };
  sys_free_mem(cmdArray);
  printf("[MPX System Shutdown]");
  return 0;
};


