/* MPX SYSTEM DECLARATIONS */


//Version identification
#define VERSION_STRING "MPX v1.1 10/21/2013\n"


//MPX file directory
#define MPXDIR "mpxfiles"

//Process stack size, in bytes
#define PCB_STACK_SIZE 1024

//Largest command size: 
#define CMDSIZE 8

//Max commands beginning with same letter:
#define CMDNUM 7

//Max Y Value taken from mpx_supt.c
#define TERMINAL_Y 24

/* setup_pcb return values */
//flags denote field init status
#define SETUP_110 -7
#define SETUP_101 -5
#define SETUP_100 -12
#define SETUP_0XX -3


#define PRIORITY 1
#define FIFO 0

//flags for pcb class and state
#define SYS 1
#define APP 2
#define RUN 1
#define READY 2
#define BLOCKED 3
#define SUSP 4

/* PROCESS CONTROL BLOCK */
typedef struct {
  char name[9];
  int class;
  int priority;
  int state;
  int isSuspended;
  unsigned char **stackSpace;
  unsigned char *stackTop;
  int memorySize;
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

/* PROCESS CONTEXT */
typedef struct {
  unsigned int BP, DI, SI, DS, ES;
  unsigned int DX, CX, BX, AX;
  unsigned int IP, CS, FLAGS;
} context;

/* PROCESS SYSTEM PARAMETERS */
typedef struct {
  int  op_code;
  int  device_id;
  unsigned char *buf_addr;
  int *count_addr;
};


/*GLOBAL QUEUES AND QUEUE DESCRIPTORS*/
extern QueueDescriptor *READYQ;
extern QueueDescriptor *BLOCKQ;


