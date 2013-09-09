#include <stdio.h>
#include "mpx_supt.h"

//largest command size, +1: 
#define CMDSIZE 9

//max commands beginning with same letter:
#define CMDNUM 2

//mpx file directory
#define MPXDIR "mpxfiles"

void cmd_mpxdir(char*);

int main() {
  int ini;
  int ck;

  //command/size/first word:
  char entry[128];
  long sz;
  char cmd[CMDSIZE];

  //table of commands:
  char ***cmdArray;

  char c;
  int i, j;

  /* Initialize system */
  ini = sys_init(MODULE_R1);
  cmdArray = sys_alloc_mem(26 * sizeof(char **));
  for (i = 0; i < 26; i++) {
    cmdArray[i] = sys_alloc_mem(CMDNUM * sizeof(char *));
    for (j = 0; j < CMDNUM; j++) {
      cmdArray[i][j] = sys_alloc_mem(CMDSIZE * sizeof(char));
    };
  };

  /* Possible command index */
  cmdArray['d'-'a'][0] = "date";
  cmdArray['d'-'a'][1] = "dir";
  cmdArray['h'-'a'][0] = "help";
  cmdArray['m'-'a'][0] = "mpxdir";
  cmdArray['q'-'a'][0] = "quit";
  cmdArray['s'-'a'][0] = "setdate";
  cmdArray['v'-'a'][0] = "version";
  
  printf("Init status: %d\n", ini);
  
  /* Begin Command Loop */
  ck = 0;
 cloop:
  while(ck >= 0) {
    ck = sys_req(READ, TERMINAL, entry, &sz);
    if (ck < 0) {
      printf("FATAL ERROR\n");
      goto stop;
    };
    
    /* Put first word of entry in cmd buffer */
    for(i = 0; i < sz; i++) {
      if (i > CMDSIZE) {
	printf("Command not found: too many characters\n");
	goto broke;
      };
      c = entry[i];
      if (isspace(c)) {
	cmd[i] = '\0';
	break;}
      else {
	cmd[i] = c;};};
    
    /* Scan for the command in cmdArray */
    for(i = 0; i < CMDNUM; i++) {
      int hit;
      for(j = 0; j < CMDSIZE; j++) { 
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
	case 'd': //run date command
	  break;
	case 'h': //run help command
	  break;
	case 'm': cmd_mpxdir(&entry[j+1]); //run mpxdir command
	  break;
	case 'q': goto stop;
	case 's': //run setdate command
	  break;
	case 'v': //run version command
	  break;
	};
      };
      case 1: {
	switch(cmd[0]) {
	case 'd': cmd_mpxdir(&entry[j+1]); //run mpxdir command
	  break;
	};
      };
      };
      if (hit == 1) break;
    retry:
    };

  broke:
  };
  
  /* Shut down the command system */
 stop:
  for (i = 0; i < 26; i++) {
    for (j = 0; j < CMDNUM; j++) {
      sys_free_mem(cmdArray[i][j]);
    };
    sys_free_mem(cmdArray[i]);
  };
  sys_free_mem(cmdArray);
  sys_exit();
  return 0;
};

void cmd_mpxdir(char *args) {
  char mpxDir[9] = MPXDIR;
  int i = 0;
  char fileName[128];
  char fileSizeString[32];
  char separator[4] = "\n";
  int sFlag = 0;
  int fileNameSize = 128;
  long fileSize;
  int ck;
  if (args[0] == '-') {
    while ((args[i+1] >= 'a') && (args[i+1] <= 'z')) {
      switch (args[++i]) {
      case 'w':  {
	sprintf(separator, "\t");
	// handle argument 'w'
	
	break; }
      case 's': {
	sFlag = 1;
	// handle argument 's' . . .
	break; }
      default:
	printf("unknown argument: %c\n", args[i]);
      };
    };
    i++;
  };
  while (isspace(args[i])) i++;
  if (args[i] != '\0') printf("unknown arguments\n");
  ck = sys_open_dir(mpxDir);
  if (ck == 0) {
    ck = sys_get_entry(&fileName[0], fileNameSize, &fileSize);
    while (ck >= 0) {
      if (sFlag > 0) { sprintf(fileSizeString, ""); }
      else { sprintf(fileSizeString, "%d bytes", fileSize); };
      printf("%s\t%s%s", fileName, fileSizeString, separator);
      ck = sys_get_entry(fileName, fileNameSize, &fileSize);
    };
  } else {
    printf("Error opening directory: %s\n", mpxDir);
  };
  sys_close_dir();
  if (separator[0] == '\t') printf("\n");
};
