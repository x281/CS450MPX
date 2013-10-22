#include <stdio.h>
// include other packages from R1 etc..

void show_all_pcb(char *pcb) // <---- correct declarations to be called???
{
	if (*pcb == NULL)
	{
		printf(" Empty...No PCBs ");
	}
	else if (*pcb != NULL)
	{
		while(pcb != 0)
		{
			printf( " List of PCBs: " ); // <---- Call the list of PCBs to print???
		
		}
		break;
	}
}

void show_pcb() // <---- correct declarations to be called???
{
	printf(" Which PCB would you like to view? : ");
	scanf("%c" , search_pcb);
	
	if(search_pcb != NULL)
	{
		// <----- somehow search for the requested PCB in the queue???
		
		printf("xxxxx") // <--- display the requested PCB???
		break;
	}
	else
	{
		printf(" The requested PCB could not be found, please try again ");
		
		return 0;
	}
}

void show_blocked(*blocked_queue, blocked_state) // <---- correct declarations to be called???
{

// ?????? Suspended and blocked ???????

	if(blocked_state == 'blocked')
	{ 
		if(*blocked_queue != NULL) // <--- Insert correct declaration of the blocked queue???
		{
		printf("xxxx"); // <--- display the blocked queues???
		break;
		}
		else if(*blocked_queue == NULL)
		{
		printf(" There are no blocked processes ")
		
		return 0;
		}
	}
	if(blocked_state == 'suspended') // <--- if suspended blocked????
	{
		if(*blocked_suspended_queue != NULL) // <--- Insert correct declaration of the blocked queue???
		{
		printf("xxxx"); // <--- display the suspended blocked queues???
		break;
		}
		else if(*blocked_suspended_queue == NULL)
		{
		printf(" There are no blocked processes ")
		
		return 0;
		}
	}
		
}

void show_ready(*ready_queue, ready_state) // <---- correct declarations to be called???
{

// ?????? Suspended and ready ??????? 

	if(ready_state == 'blocked')
	{ 
		if(*ready_queue != NULL) // <--- Insert correct declaration of the ready queue???
		{
		printf("xxxx"); // <--- display the ready processes???
		break;
		}
		else if(*ready_queue == NULL)
		{
		printf(" There are no ready processes ")
		
		return 0;
		}
	}
	if(ready_state == 'suspended') // <--- if suspended ready???
	{
		if(*ready_suspended_queue != NULL) // <--- Insert correct declaration of the suspended ready queue???
		{
		printf("xxxx"); // <--- display the suspended ready processes???
		break;
		}
		else if(*ready_suspended_queue == NULL)
		{
		printf(" There are no ready processes ")
		
		return 0;
		}
	}
		
}
		
		
	
		
			