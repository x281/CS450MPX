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
  cmdArray['c'-'b'][0] = "block";\
  cmdArray['c'-'a'][0] = "clear";\
  cmdArray['d'-'a'][0] = "date";\
  cmdArray['d'-'a'][1] = "dir";\
  cmdArray['h'-'a'][0] = "help";\
  cmdArray['m'-'a'][0] = "mpxdir";\
  cmdArray['m'-'a'][1] = "mkproc";\
  cmdArray['q'-'a'][0] = "quit";\
  cmdArray['r'-'a'][0] = "random";\
  cmdArray['r'-'a'][1] = "rmproc";\
  cmdArray['r'-'a'][2] = "resume";\
  cmdArray['s'-'a'][0] = "setdate";\
  cmdArray['s'-'a'][1] = "suspend";\
  cmdArray['s'-'a'][2] = "setp";\
  cmdArray['s'-'a'][3] = "showpcb";\
  cmdArray['s'-'a'][4] = "showall";\
  cmdArray['s'-'a'][5] = "showrdy";\
  cmdArray['s'-'a'][6] = "showblk";\
  cmdArray['t'-'a'][0] = "test";\
  cmdArray['u'-'a'][0] = "unblock";\
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
void  cmd_block(char*);
void  cmd_unblock(char*);
void  cmd_mkproc(char*);
void  cmd_rmproc(char*);
void  cmd_suspend(char*);
void  cmd_resume(char*);
void  cmd_setp(char*);
void  cmd_showpcb(char*);
void  cmd_showall(char*);
void  cmd_showrdy(char*);
void  cmd_showblk(char*);

/* prng.c */
long  rnd();
int   randi(int);
float randf();
void  rand_init();

/* pcb.c */
ProcessControlBlock* allocate_pcb();
                int  free_pcb(ProcessControlBlock*);
               void  prioritize(ProcessControlBlock*, int);
                int  setup_pcb(ProcessControlBlock*, char*, int);
ProcessControlBlock* find_pcb(char*);
                int  insert_pcb(ProcessControlBlock*, QueueDescriptor*, int);
                int  remove_pcb(ProcessControlBlock*, QueueDescriptor*);

void cmd_test();

///////////////////
/* MAIN FUNCTION */
///////////////////
int main() {
  int ini;

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

  QueueDescriptor readyQueue;
  QueueDescriptor blockedQueue;

  char c;   //  generic //
  int i, j; //variables //

  char pa[3] = "[]";//re:cmd//
  char pb[3] = "()";//prompt//

  /* Initialize memory */
  cmdArray = malloc(26 * sizeof(char **));
  for (i = 0; i < 26; i++) {
    cmdArray[i] = malloc(CMDNUM * sizeof(char *));
    for (j = 0; j < CMDNUM; j++) {
      cmdArray[i][j] = malloc((CMDSIZE + 1) * sizeof(char));
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
   // BLOCK PROCESS:
	case 'b': cmd_block(&entry[j+1+spaceCount]);			   
	  break;
   // CLEAR:
	case 'c': cmd_clear();
	  break;
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
	  int i;
	  char name[9] = "testing";
	  ProcessControlBlock *testpcb;


	  testpcb = allocate_pcb();
	  printf("Test status: %d\n", setup_pcb(testpcb, name, APP));
	  for(i = 0; i < PCB_STACK_SIZE; i++) {
	    printf("%c", testpcb->stackSpace[i]);
	  };
	  printf("\n");
	  free_pcb(testpcb);
	};
	  break;
   // UNBLOCK PROCESS:
	case 'u': cmd_unblock(&entry[j+1+spaceCount]);
	  break;
   // VERSION:
	case 'v': cmd_version();
	  break;
	};
	break;
      };
      case 1: {
	switch(cmd[0]) {
   // DIR:  
	case 'd': cmd_mpxdir(&entry[j+1+spaceCount]);
	  break;
   // MKPROC (MAKE PROCESS):
	case 'm': cmd_mkproc(&entry[j+1+spaceCount]);
	  break;
   // RMPROC (REMOVE PROCESS):
	case 'r': cmd_rmproc(&entry[j+1+spaceCount]);
	  break;
   // SUSPEND PROCESS:
	case 's': cmd_suspend(&entry[j+1+spaceCount]);
	  break;
	};
	break;
      };
      case 2: {
	switch(cmd[0]) {
   // RESUME PROCESS:
	case 'r': cmd_resume(&entry[j+1+spaceCount]);
	  break;
   // SET PRIORITY:
	case 's': cmd_setp(&entry[j+1+spaceCount]);
	  break;
	};
	break;
      };
      case 3: {
	switch(cmd[0]) {
   // SHOW PCB:
	case 's': cmd_showpcb(&entry[j+1+spaceCount]);
	  break;
	};
	break;
      };
      case 4: {
	switch(cmd[0]) {
   // SHOW ALL:
	case 's': cmd_showall(&entry[j+1+spaceCount]);
	  break;
	};
	break;
      };
      case 5: {
	switch(cmd[0]) {
   // SHOW READY:
	case 's': cmd_showrdy(&entry[j+1+spaceCount]);
	  break;
	};
	break;
      };
      case 6: {
	switch(cmd[0]) {
   // SHOW BLOCKED:
	case 's': cmd_showblk(&entry[j+1+spaceCount]);
	  break;
	};
	break;
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
  printf("[MPX System Shutdown]\n");
  return 0;
};


