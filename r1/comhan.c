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
  
  //Date
  date_rec *day;

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
  sys_req(CLEAR, TERMINAL, entry, &sz);
  /* Possible command index */
  cmdArray['c'-'a'][0] = "clear";
  cmdArray['d'-'a'][0] = "date";
  cmdArray['d'-'a'][1] = "dir";
  cmdArray['h'-'a'][0] = "help";
  cmdArray['m'-'a'][0] = "mpxdir";
  cmdArray['q'-'a'][0] = "quit";
  cmdArray['s'-'a'][0] = "setdate";
  cmdArray['v'-'a'][0] = "version";
  
  
  //printf("Init status: %d\n", ini);
  
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
	case 'd': cmd_disp_date(day);
	  break;
	case 'h': cmd_help();
	  break;
	case 'm': cmd_mpxdir(&entry[j+1]); //run mpxdir command
	  break;
	case 'q': goto stop;
	  
	case 's': cmd_set_date(day, &entry[j+1]);
	  break;
	case 'v': cmd_version();
	  break;
	case 'c': cmd_clear();
	  break;
	};
	break;
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
//Version command function
int cmd_version() {
printf("MPX v1.0 9/10/2013\n");
}
//Help command function
int cmd_help() {
printf("Commands:\nclear -- clears the terminal screen\ndate -- Displays the current system date\ndir -- displays the current directory\nhelp -- lists all of the commands and their functionality\nquit -- exits the terminal\nsetdate -- sets the new system date, entry syntax is DD-MM-YYYY\nversion -- displays current MPX version\n");
}
//Display date command function
int cmd_disp_date(date_rec *date) {
sys_get_date(date);
printf("%d-%d-%d\n",date->month, date->day, date->year);
}
//Set date command function
int cmd_set_date(date_rec *date, char* newdate) {
char dd[3];
char mm[3];
char yyyy[5];
int successful = 1;  //pseudo boolean value for syntax checking
int errordate = 0;   //pseudo boolean value to make sure the dates entered do not violate the laws of time and space
int numcheck;

if(strlen(newdate) != 11 || newdate[2] != '-' || newdate[5] != '-') { //format checking
successful = 0;
goto end;
}

for(numcheck = 0; numcheck < strlen(newdate)-1; numcheck++) { //checking to see that the date only contains numbers
	if(numcheck != 2 && numcheck != 5) {
		if(!isdigit(newdate[numcheck])) {
		successful = 0;
		goto end;
		}
	}
}

mm[0] = newdate[0]; //reading the date string into easier to handle pieces
mm[1] = newdate[1];
mm[2] = '\0';
dd[0] = newdate[3];
dd[1] = newdate[4];
dd[2] = '\0';
yyyy[0] = newdate[6];
yyyy[1] = newdate[7];
yyyy[2] = newdate[8];
yyyy[3] = newdate[9];
yyyy[4] = '\0';

if(atoi(mm) > 12) {  //making sure you aren't living out on jupiter or anything
errordate = 1;
goto end;
}
switch(atoi(mm)) { 
case 1,3,5,7,8,10,12:
	if(atoi(dd) > 31) {
		errordate = 1;
		goto end;
	}
case 4,6,9,11:
	if(atoi(dd) > 30) {
		errordate = 1;
		goto end;
	}
case 2:
	if(atoi(dd) > 29 && atoi(yyyy) % 4 == 0) {
		errordate = 1;
		goto end;
	} else if(atoi(dd) > 28 && atoi(yyyy) % 4 != 0) {
		errordate = 1;
		goto end;
	}
}

date->day = atoi(dd);  //converts the system date into the new one
date->month = atoi(mm);
date->year = atoi(yyyy);
sys_set_date(date);
end:
if(!successful) { //errors
printf("ERROR: Incorrect date format please use numbers in the format of MM-DD-YYYY\n");
}
if(errordate) {
printf("ERROR: Date is numerically incorrect\n");
}
return successful;
}

int cmd_clear() { //clears the screen, because I hate having smutt all over my screen
int y;
char x[1];
sys_req(CLEAR, TERMINAL, x, &y);
return 1;
}

