/* MPX SYSTEM DECLARATIONS */


//Version identification
#define VERSION_STRING "MPX v1.02 9/21/2013\n"


//MPX file directory
#define MPXDIR "mpxfiles"



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



/* PROCESS CLASSES & STATES */
typedef enum {sys_proc, pgm_proc} proc_class;
typedef enum {running_s, ready_s, blocked_s} proc_state;

/* PROCESS CONTROL BLOCK */
typedef struct {
           char  name[9];
     proc_class  class;
           int   priority;
     proc_state  state;
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
   ProcessControlBlock  pcb;
} ProcessRecord;


/* QUEUE DESCRIPTOR */
typedef struct {
  ProcessRecord *head;
  ProcessRecord *tail;
} QueueDescriptor;

