#include <stdio.h>
#include "mpx_supt.h"
#include "mpx.h"


////////////////////////
/* COMMAND FUNCTIONS: */
////////////////////////

//////////////
//MPX file directory fn
void cmd_mpxdir(char *args) {
  char mpxDir[128] = MPXDIR;
  int i = 0;

  //helpers for sys fn
  char fileName[128];
  char fileSizeString[32];
  int fileNameSize = 128;
  long fileSize;

  //format control 
  char separator[4] = "\n";
  int sFlag = 0;
  int pFlag = 0;
  int linesPrinted   = 0;
  int entriesPrinted = 0;

  long bytesTotal = 0L;

  int ck; //status

  if ((args[i] == '-') || (args[i] == '\/')) {
    while ((args[i+1] >= 'a') && (args[i+1] <= 'z')) {
      /* Argument Handling */
      switch (args[++i]) {
   //'Wide' format, multiple entries per line:
      case 'w':  {
	sprintf(separator, "  ");// replace \n
	break; }
   //'Pause' output at end of each page:
      case 'p' : {
	pFlag = 1;
	linesPrinted = 0;
	break;}
   //'Short' output, no size info
      case 's': {
	sFlag = 1;
	break; }
   //'Help' access [bypasses cmd]:
      case '\?':
      case 'h': { 
	printf("Supported arguments:\n"
	       "  -h : Help (display this information)\n"
	       "  -p : Pause at end-of-page (for long listings)\n"
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

  /* Read in subdirectory */
  if (args[i] != '\0') {      
    if (args[i] != '\\') strcat(mpxDir, "\\");
    strcat(mpxDir, &args[i]);         //add subdirectory
    mpxDir[strlen(mpxDir) - 1] = '\0';//trim newline
  };

  ck = sys_open_dir(mpxDir);
  entriesPrinted = 0;
  bytesTotal = 0L;

  //Import and print directory contents
  if (ck == 0) {
    ck = sys_get_entry(&fileName[0], fileNameSize, &fileSize);

    if (ck == 0) printf(".MPX files in \\%s\\\n", mpxDir);
    if (sFlag && (separator[0] != '\n')) printf("  ");

    while (ck >= 0) {//Loop if file found
      if (sFlag > 0) { sprintf(fileSizeString, "\t"); }
      else { sprintf(fileSizeString, "%10dK", 1 + (fileSize / 1024)); };
      if (sFlag == 0) {
	int k;
	for(k = 0; k < strlen(fileSizeString); k++) {
	  if (isspace(fileSizeString[k])) fileSizeString[k] = '.';
	};
      };
      printf(" %9s%s%s", fileName, fileSizeString, separator);
      // printf("\n\t%d\t%ld\n", entriesPrinted, bytesTotal);
      entriesPrinted = entriesPrinted + 1;
      bytesTotal = bytesTotal + fileSize;
      if (separator[0] == '\n') { linesPrinted++; }
      else {
	if (((entriesPrinted % 3) == 0) &&
	    (sFlag == 0)) { 
	  linesPrinted++; 
	  printf("\n");
	}
	else if (((entriesPrinted % 5) == 0) &&
		 (sFlag > 0)) {
	  linesPrinted++;
	};
      };
      if (pFlag > 0) {
	if (((linesPrinted % TERMINAL_Y) == 0) &&
	    (((sFlag == 0) && 
	      (((entriesPrinted % 3) == 0) || 
	       (separator[0] == '\n'))) ||
	     ((sFlag > 0) && 
	      (((entriesPrinted % 5) == 0) ||
	       (separator[0] == '\n'))))) {
	  printf("Press any key to continue...\n");
	  getch();
	};
      };
      ck = sys_get_entry(fileName, fileNameSize, &fileSize);
    };
  } else {
    printf("Error opening directory: %s", mpxDir);
    if (mpxDir[strlen(mpxDir) - 1] != '\n') printf("\n");
  };
  
  
  if ((separator[0] == ' ') &&
      (((sFlag == 0) && ((entriesPrinted % 3) != 0)) ||
       ((sFlag > 0) && ((entriesPrinted % 5) != 0)))) printf("\n");
  if (entriesPrinted > 0) {
    printf("\t   Total bytes: %ld\n"
	   "\t   Total files: %d\n",
	   bytesTotal, entriesPrinted);
  };
  
  sys_close_dir();

 skip:
}; 


//////////////
//Version command function
void cmd_version() {
  printf( VERSION_STRING );
}


//////////////
//Help command function
void cmd_help() {
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
void cmd_disp_date(date_rec *date) {
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
    printf("ERROR: Incorrect date format, "
	   "please use numbers in the format of MM-DD-YYYY\n");
  }
  if(errordate) {
    printf("ERROR: Date is numerically incorrect\n");
  }
  return successful;
}


//////////////
//Clear screen function
void cmd_clear() { 
  int y;
  char x[1];
  sys_req(CLEAR, TERMINAL, x, &y);
}
