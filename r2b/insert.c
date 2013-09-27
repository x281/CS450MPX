void insert_pcb (ProcessRecord *queue, ProcessControlBlock *newpcb)
{
   ProcessRecord newPtr, previousPtr, currentPtr;

   newPtr = sys_alloc_mem(sizeof(ProcessRecord));
    if (newPtr != NULL) {    
      newPtr = newpcb;
      newPtr->next = NULL;

      previousPtr = NULL;
      currentPtr = *queue;

      while (currentPtr != NULL) {
         previousPtr = currentPtr;          
         currentPtr = currentPtr->nextPtr;  
      }

      if (previousPtr == NULL) {
         newPtr->next = *queue;
         *queue = newPtr;
      }
      else {
         previousPtr->nextPtr = newPtr;
         newPtr->next = currentPtr;
         newPtr->prev = previousPtr;
      }

} else {
      printf("Process not inserted. No memory available.\n");
   }
}
