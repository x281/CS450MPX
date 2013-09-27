/* MPX SYSTEM DECLARATIONS */


//Version identification
#define VERSION_STRING "MPX v1.02 9/21/2013\n"


//MPX file directory
#define MPXDIR "mpxfiles"

//Process stack size, in bytes
#define PCB_STACK_SIZE 1024

//Largest command size: 
#define CMDSIZE 8

//Max commands beginning with same letter:
#define CMDNUM 2

//Max Y Value taken from mpx_supt.c
#define TERMINAL_Y 24

/* setup_pcb return values */
//flags denote field init status
#define SETUP_110 -7
#define SETUP_101 -5
#define SETUP_100 -12
#define SETUP_0XX -3

//flags for pcb class and state
#define SYS 1;
#define APP 2;
#define RUN 1;
#define READY 2;
#define BLOCK 3;
#define SUSP 4;

/* PROCESS CONTROL BLOCK */
typedef struct {
           char  name[9];
           int   class;
           int   priority;
           int   state;
           int   isSuspended;
  unsigned char *stackSpace;
  unsigned char *stackTop;
           int   memorySize;
  unsigned char *load_addr;
  unsigned char *exec_addr;
} ProcessControlBlock;

/* PCB RECORD */
typedef struct {
  struct ProcessRecord *next;
  struct ProcessRecord *prev;
   ProcessControlBlock *pcb;
} ProcessRecord;


/* QUEUE DESCRIPTOR */
typedef struct {
  ProcessRecord *head;
  ProcessRecord *tail;
} QueueDescriptor;

