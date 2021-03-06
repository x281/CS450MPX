#include <stdio.h>
#include "mpx_supt.h"

/****************************

  CS 450 MPX Operating System
  Module R1

  TEAM TEAMWORK
  Erik Bernard
  James Epting
  Chris Ho
  Darren Ringer
  
  **************************/


//Version identification
#define VERSION_STRING "MPX v1.0 9/13/2013\n"


//Largest command size: 
#define CMDSIZE 8

//Max commands beginning with same letter:
#define CMDNUM 2


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
  cmdArray['v'-'a'][0] = "version";


//MPX file directory
#define MPXDIR "mpxfiles"

//Park-Miller PRNG support
#define RNG_M 2147483647L /* m = 2^31 - 1 */
#define RNG_A 16807L
#define RNG_Q 127773L     /* m div a */
#define RNG_R 2836L       /* m mod a */
static long rnd_seed;

//Function Prototypes
int mpx_command_loop();
void cmd_mpxdir(char*);
int cmd_version();
int cmd_help();
int cmd_set_date(date_rec*, char*);
int cmd_disp_date(date_rec*);
int cmd_clear();
long rnd();
int randi(int);
float randf();


///////////////////
/* MAIN FUNCTION */
///////////////////
int main() {
  int ini;

  //Setup RNG
  long seedval = 1234567890L;
  rnd_seed = (seedval % (RNG_M - 1)) + 1;

  //Init sys helper
  ini = sys_init(MODULE_R1);

  //Start cmd loop
  if (ini == 0) {
    mpx_command_loop();

    //Shutdown sys helper
    sys_exit();

  } else printf("SYSTEM ERROR: Unable to initialize library");

  return 0;
};

/////////////////
//Command loop //
/////////////////
int mpx_command_loop() {
  int ck;

  char entry[128];  //user input
  long sz;          //length
  char cmd[CMDSIZE + 1];//1st word
  
  date_rec *day;    //date

  char ***cmdArray; //table of cmds

  char c;  //  generic
  int i, j;//variables

  char pa[3] = "[]";//re:cmd
  char pb[3] = "()";//prompt

  /* Initialize memory */
  cmdArray = sys_alloc_mem(26 * sizeof(char **));
  for (i = 0; i < 26; i++) {
    cmdArray[i] = sys_alloc_mem(CMDNUM * sizeof(char *));
    for (j = 0; j < CMDNUM; j++) {
      cmdArray[i][j] = sys_alloc_mem((CMDSIZE + 1) * sizeof(char));
    };
  };
  sys_req(CLEAR, TERMINAL, entry, &sz);

  /* Possible command index */
  CMD_IDX  
  
  /* Begin Command Loop */
  ck = 0;
 cloop:
  while(ck >= 0) {
    printf("%c%c> ", pa[randi(2)], pb[randi(2)]); //cmd prompt
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
	case 'd': cmd_disp_date(day);
	  break;
	case 'h': cmd_help();
	  break;
	case 'm': cmd_mpxdir(&entry[j+1]);
	  break;
	case 'q': goto stop;
	
	case 'r': printf("%d\n", randi(atoi(&entry[j+1])));
	  break;
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
	case 'd': cmd_mpxdir(&entry[j+1]);
	  break;
	};
      };
      };
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
  return 0;
};


////////////////////////
/* COMMAND FUNCTIONS: */
////////////////////////

//////////////
//MPX file directory fn
void cmd_mpxdir(char *args) {
  char mpxDir[9] = MPXDIR;
  int i = 0;

  //helpers for sys fn
  char fileName[128];
  char fileSizeString[32];
  int fileNameSize = 128;
  long fileSize;

  //format control 
  char separator[4] = "\n";
  int sFlag = 0;

  int ck;//status

  if (args[0] == '-') {
    while ((args[i+1] >= 'a') && (args[i+1] <= 'z')) {
      /* Argument Handling */
      switch (args[++i]) {
      case 'w':  {// wide format, continuous line out
	sprintf(separator, "\t");// replace \n with \t
	break; }
      case 's': {// short format, no size information
	sFlag = 1;// suppress size string output
	break; }
      case 'h': {// help request
	printf("Supported arguments:\n"
	       "  -h : Help (display this information)\n"
	       "  -s : Short format (no size info)\n"
	       "  -w : Wide format (no line breaks)\n");
	goto skip; }
      default:
	printf("Unknown argument: %c\n", args[i]);
      };
    };
    i++;
  };
  while (isspace(args[i])) i++;
  if (args[i] != '\0') {
    printf("Unknown argument(s) ['dir -h' for help]\n");
  };

  ck = sys_open_dir(mpxDir);

  //Import and print directory contents
  if (ck == 0) {
    ck = sys_get_entry(&fileName[0], fileNameSize, &fileSize);
    while (ck >= 0) {//Loop if file found
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
 skip:
}; 


//////////////
//Version command function
int cmd_version() {
printf( VERSION_STRING );
}


//////////////
//Help command function
int cmd_help() {
printf("Commands:\n"
       "clear -- clears the terminal screen\n"
       "date -- Displays the current system date\n"
       "dir -- displays the current directory ['dir -h' for more help]\n"
       "help -- lists all of the commands and their functionality\n"
       "quit -- exits the terminal\n"
       "setdate -- sets the new system date, entry syntax is DD-MM-YYYY\n"
       "version -- displays current MPX version\n");
}


//////////////
//Display date command function
int cmd_disp_date(date_rec *date) {
sys_get_date(date);
printf("%d-%d-%d\n",date->month, date->day, date->year);
}


//////////////
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


//////////////
//Clear screen function
int cmd_clear() { 
int y;
char x[1];
sys_req(CLEAR, TERMINAL, x, &y);
return 1;
}


////////////////////////////////////////////////
/* Park-Miller pseudo-random number generator */
////////////////////////////////////////////////
long rnd() {
  long low, high, test;

  high = rnd_seed / RNG_Q;
  low = rnd_seed % RNG_Q;
  test = RNG_A * low - RNG_R * high;
  if (test > 0)
    rnd_seed = test;
  else
    rnd_seed = test + RNG_M;
  return rnd_seed;
}

//////////////
//Random int from 0 to lim-1:
int randi(int lim) {
  return (int)((float)lim * rnd() / (RNG_M - 1.0));
}

//////////////
//Random float 0>-1.0
float randf() {
  return (float)(rnd() / (RNG_M - 1.0));
}
