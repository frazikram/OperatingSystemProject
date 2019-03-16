//First letinclude all the required system libraries.
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

//Global Variables

long RQ;
long OSFreeList;
long UserFreeList;

//Main Function
int main()
{
	/*
	// Run until shutdown

	Call Initialize System function;

	while (not shutdown)
	{
		// Check and process interrupt
		call Check and Process Interrupt function and store return status
		if interrupt is shutdown, exit from main;

		// Dump RQ and WQ
		DumpRQ(“RQ: Before CPU scheduling\n”);
		DumpWQ(“WQ: Before CPU scheduling\n”);
		DumpMemory(“Dynamic Memory Area before CPU scheduling\n”);

		// Select next process from RQ to give CPU
		Running PCB ptr = Select Process From RQ ();  // call the function

		// Perform restore context using Dispatcher
		Call Dispatcher function with Running PCB ptr as argument;

		Dump RQ(“RQ: After selecting process from RQ\n”);
		Dump Running PCB and CPU Context passing  Running PCB ptr as argument;

		// Execute instructions of the running process using the CPU
		status = CPU Execute Program ();  // call the function

		// Dump dynamic memory area
		Dump dynamic memory calling Dump Memory function(“After execute program\n”);

		// Check return status – reason for giving up CPU
		if(status is Time slice Expired)
		{
			Save CPU Context of running process in its PCB;  // running process is losing CPU
			Insert running process PCB into RQ;
			Set Running PCB ptr = End Of List;
		}
		else if((status is Process Halted) OR (status < 0))  // Halt or run-time error
		{
			Terminate running Process;
			Set Running PCB ptr = End Of List ;
		}
		else if(status is Start Of Input Operation)		// io_getc
		{
			Set reason for waiting in the running PCB to Input Completion Event;
			Insert running process into WQ;
			Set Running PCB ptr = End Of List;
		}
		else if(status == Start Of Output Operation)		// io_putc
		{
			Set reason for waiting in the running PCB to Input Completion Event;
			Insert running process into WQ;
			Set Running PCB ptr = End Of List;
		}
		else
		{	// Unknown programming error
			Display Unknown programming error message;
		 }
	}  // end of while not shutdown loop

	Print OS is shutting down message;
	*/
	Return(status);  // Terminate Operating System
}  // end of main function

//Create Process
long CreateProcess (char &filename, long priority)	// or char * pointer
{
	/*
	// Allocate space for Process Control Block
      Set PCBptr = Allocate OS Memory for PCB;  // return value contains address or error
      Check for error and return error code, if memory allocation failed

      // Initialize PCB: Set nextPCBlink to end of list, default priority, Ready state, and PID
      Initialize PCB passing PCBptr as argument;
 
      // Load the program
      Set value =load the program calling by Absolute Loader passing filename as argument;
      Check for error and return error code, if loading program failed

      // store PC value in the PCB of the process
     Set PC value in the PCB = value;  

      // Allocate stack space from user free list
      Set ptr = Allocate User Memory of size StackSize;
       if (ptr < 0)	// check for error
      {  // User memory allocation failed
	Free allocated PCB space by calling Free OS Memory passing PCBptr and  PCBsize;
	return(ptr);  // return error code
      }

      // Store stack information in the PCB – SP, ptr, and size
      Set SP in the PCB = ptr + Stack Size;  // empty stack is high address, full is low address
      Set stack start address in the PCB to ptr;
      Set stack size in the PCB = Stack Size;

      Set priority in the PCB = priority;	// Set priority

     Dump program area;

     Print PCB passing PCBptr; 

     // Insert PCB into Ready Queue according to the scheduling algorithm
     Insert PCB into Ready Queue passing PCBptr;
	*/
     return(OK);
}  // end of CreateProcess() function

//Initialize PCB

void InitializePCB (long PCBptr)
{
     /* Set entire PCB area to 0 using PCBptr;	// Array initialization
     
     // Allocate PID and set it in the PCB. PID zero is invalid
     Set PID field in the PCB to = ProcessID++;  // ProcessID is global variable initialized to 1

     Set priority field in the PCB = Default Priority;  // DefaultPriority is a constant set to 128
     Set state field in the PCB = ReadyState;    // ReadyState is a constant set to 1
     Set next PCB pointer field in the PCB = EndOfList;  // EndOfList is a constant set to -1

     */
     return;
}  // end of InitializePCB

//Print PCB values

Void PrintPCB(long PCBptr)
{
	/*Print the values of the following fields from PCB with a text before the value like below:
		PCB address = 6000, Next PCB Ptr = 5000, PID = 2, State = 2, PC = 200, SP = 4000, 
Priority = 127, Stack info: start address = 3990, size = 10
		GPRs = print 8 values of GPR 0 to GPR 7
		*/
}  // end of PrintPCB() function

//Print Given Queue

long PrintQueue (long Qptr)
{
	// Walk thru the queue from the given pointer until end of list
	// Print each PCB as you move from one PCB to the next
/*
	Declare long currentPCBPtr = Qptr;

	if(currentPCBPtr is End of List)
	{
		Display empty list message;
		return(OK);
	}

	// Walk thru the queue
	While(currentPCBPtr is not equal to End of List)
	{
		Print PCB passing currentPCBPtr;
		Set currentPCBPtr = next PCB pointer using currentPCBPtr;
	}  // end of while loop
*/
	return (OK);
}  // end of PrintQueue() function


//Insert PCB into ready Queue

long InsertIntoRQ (long PCBptr)
{
	/*
	// Insert PCB according to Priority Round Robin algorithm
	// Use priority in the PCB to find the correct place to insert.
long previousPtr = EndOfList;
long currentPtr = RQ;

	// Check for invalid PCB memory address
     if((PCBptr < 0) || (PCBptr >MaxMemoryAddress))
     {
	display invalid address error message;
	return(ErrorInvalidMemoryAddress);  // Error code < 0
     }

     Memory[PCBptr + StateIndex] = Ready;		// set state to ready state
     Memory[PCBptr + NextPointerIndex] = EndOfList;	 // set next pointer to end of list

     if( RQ == EndOfList)	// RQ is empty
     {
	RQ = PCBptr;
	return(OK);
     }

	// Walk thru RQ and find the place to insert
	// PCB will be inserted at the end of its priority

     while( currentPtr != EndOfList)
     {
if(Memory[PCBptr + PriorityIndex] > Memory[currentPtr + PriorityIndex])
{	// found the place to insert
       if(previousPtr == EndOfList)
       {
	// Enter PCB in the front of the list as first entry
	Memory[PCBptr + NextPCBPointerIndex] = RQ;
	RQ = PCBptr;
	return(OK);
       }
	// enter PCB in the middle of the list
       Memory[PCBptr +NextPCBPointerIndex] = Memory[previousPtr + NextPCBPointerIndex];
       Memory[previousPtr + NextPCBPointerIndex] = PCBptr;
       return(OK);
}
else  // PCB to be inserted has lower or equal priority to the current PCB in RQ
{	// go to the next PCB in RQ
	previousPtr = currentPtr;
	currentPtr = Memory[currentPtr+NextPCBPointerIndex];
}
     }  // end of while loop

     // Insert PCB at the end of the RQ
     Memory[previousPtr + NextPointerIndex] = PCBptr;
     */
     return(OK); 

}  // end of InsertIntoRQ() function

//Insert PCB into waiting queue

long InsertIntoWQ (long PCBptr)
{
	/*
	// Insert the given PCB at the front of WQ

	// Check for invalid PCB memory address
if((PCBptr < 0) || (PCBptr >MaxMemoryAddress))
     	{
		display invalid PCB address error message;
		return(ErrorInvalidMemoryAddress);  // Error code < 0
     	}

Memory[PCBptr + StateIndex] = Waiting;	// set state to ready state
Memory[PCBptr + NextPointerIndex] = WQ;	 // set next pointer to end of list

WQ = PCBptr;
*/
     	return (OK);
}  // end of InsertIntoWQ () function

//Select first process from RQ to Give CPU
long SelectProcessFromRQ()
{
	/*
	Declare PCBptr as type long and initialize to RQ;  // first entry in RQ

	if(RQ != End of List)
	{
	      // Remove first PCB from RQ
	     Set RQ = Next PCB pointed by RQ;
	}

	// Set next point to EOL in the PCB
            Set Next PCB field in the given PCB to End Of List;
            */

	return(PCBptr);
}  // end of SelectProcessFromRQ() function

//Save CPU context into Running Process PCB
void SaveContext(long PCBptr)
{
	// Assume PCBptr is a valid pointer.
	/*

	Copy all CPU GPRs into PCB using PCBptr with or without using loop

	Set SP field in the PCB = SP;  	// Save SP
	Set PC field in the PCB = PC;	// Save PC
	*/
	return;
}  // end of SaveContext() function

//Restore CPU context from the given PCB
void Dispatcher(long PCBptr)
{
	// PCBptr is assumed to be correct.

	// Copy CPU GPR register values from given PCB into the CPU registers
	// This is opposite of save CPU context

	// Restore SP and PC from given PCB

	// Set system mode to User mode
	//PSR = UserMode;	// UserMode is 2, OSMode is 1.

	return;
}  // end of Dispatcher() function


//Terminate Process

Void TerminateProcess (long PCBptr)
{
	// Return stack memory using stack start address and stack size in the given PCB

	// Return PCB memory using the PCBptr

	return;
}  // end of TerminateProcess function()

//Allocate OS Memory

long AllocateOSMemory (long RequestedSize);  // return value contains address or error
{
	/*
	// Allocate memory from OS free space, which is organized as link
     if(OSFreeList == EndOfLisrt)
     {
	display no free OS memory error;
	return(ErrorNoFreeMemory);   // ErrorNoFreeMemory is constant set to < 0
      }
     if(RequestedSize < 0)
     {
	display invalid size error;
	return(ErrorInvalidMemorySize);  // ErrorInvalidMemorySize is constant < 0
     }
      if(RequestedSize == 1)
	RequestedSize = 2;  // Minimum allocated memory is 2 locations

      CurrentPtr = OSFreeList;
       PreviousPtr = EOL;
      while (CurrentPtr != EndOfList)
      {
	// Check each block in the link list until block with requested memory size is found
	if(Memory[CurrentPtr + 1] == RequestedSize)
	{  // Found block with requested size.  Adjust pointers
	      if(CurrentPtr == OSFreeList)  // first block
	      {
		OSFreeList = Memory[CurrentPtr];  // first entry is pointer to next block
		Memory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
		Return(CurrentPtr);	// return memory address
	      }
	      else  // not first black
	      {
		Memory[PreviousPtr] = Memory[CurrentPtr];  // point to next block
		Memory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
		return(CurrentPtr);    // return memory address
	      }
               }
     	else if(Memory[CurrentPtr + 1] > RequestedSize)
	{  // Found block with size greater than requested size
	      if(CurrentPtr == OSFreeList)  // first block
	      {
		Memory[CurrentPtr + RequestedSize] = Memory[CurrentPtr];  // move next block ptr
		Memory[CurrentPtr + RequestSize + 1] = Memory[CurrentPtr +1] – RequestedSize;
		OSFreeList = CurrentPtr + RequestedSize;  // address of reduced block
		Memory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
		return(CurrentPtr);	// return memory address
	      }
	      else  // not first black
	      {
		Memory[CurrentPtr + RequestedSize] = Memory[CurrentPtr];  // move next block ptr
		Memory[CurrentPtr + RequestSize + 1] = Memory[CurrentPtr +1] – RequestedSize;
		Memory[PreviousPtr] = CurrentPtr + RequestedSize;  // address of reduced block
		Memory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
		return(CurrentPtr);	// return memory address
	      }
	}
	else  // small block 
	{  // look at next block
	      Previousptr = CurrentPtr;
	       CurrentPtr = Memory[CurrentPtr];
	}
      } // end of while CurrentPtr loop

      display no free OS memory error;
      return(ErrorNoFreeMemory);   // ErrorNoFreeMemory is constant set to < 0
      */
}  // end of AllocateOSMemory() function

//Free OS Memory
long FreeOSMemory (long ptr, long size);  // return value contains OK or error code
{
    /*
     if (ptr is outside the OS free list area)  	// Address range is given in the class
     {
	display invalid address error message;
	return(ErrorInvalidMemoryAddress);  // ErrorInvalidMemoryAddress is constantset to  < 0
     }
    
     if(size is 1)	// check for minimum allocated size, which is 2 even if user asks for 1 location
     {
	Set size = 2;  // minimum allocated size
      }
     else if( size is less than 1) OR ((ptr+size) greater than or equal to Maximum Memory Address))
     {	// Invalid size
	display invalid size or address error message;
	return(ErrorInvalidSizeORMemoryAddress);  	// All error codes are < 0
     } 

	// Return memory to OS free space.  Insert at the beginning of the link list
//    Insert the given free block at the beginning of the OS free list;
    Make the given free block point to free block pointed by OS free List;
    Set the free block size in the given free block;
    Set OS Free List point to the given free block;
    */

     return (OK);
}  // end of FreeOSMemory() function

//Allocate User Memory

Long AllocateUserMemory (long size);  // return value contains address or error code
{
	// Allocate memory from User free space, which is organized as link
	// copy OS code and modify to use UserFreeList
}  // end of AllocateUserMemory() function

//Free User Memory

long FreeUserMemory (long ptr,  long size);  // return value contains OK or error code
{
	/*	
	// Return memory to User free space.  
// Insert the returned free block at the beginning of the link list

	// This code is similar to free OS memory

     If (ptr out side the free user memory area )  // user memory area is given in the class
     {
	display invalid address error message;
	return(ErrorInvalidMemoryAddress);  // ErrorInvalidMemoryAddress is constantset to  < 0
     }
	// Check for invalid size and minimum size
     if(size is 1)	// check for minimum allocated size, which is 2 even if user asks for 1 location
     {
	Set size = 2;  // minimum allocated size
      }
     else if((size < 1) OR ((ptr+size) outside maximum free user memory area))	// invalid size
     {	// Invalid size
	display invalid size or address error message;
	return(ErrorInvalidSizeORMemoryAddress);  	// All error codes are < 0
     } 
	// Insert the free block at the beginning of the link list
	// Code is similar to free OS free block
	*/

     return (OK);
}  // end of FreeUserMemory() function

//Check and Process Interrupt

Void CheckAndProcessInterrupt()
{
	/*
	// Prompt and read interrupt ID
	Display possible interrupts: 	// 0 – no interrupt
					// 1 – run program
					// 2 – shutdown system
					// 3 – Input operation completion (io_getc)
					// 4 – Output operation completion (io_putc)
	Read interrupt ID;
              Display the interrupt value that was read;
	
	// Process interrupt
	switch(InterruptID)
	{
		case 0:  	// No interrupt
			break;

		case 1: 	// Run program
			call ISR run Program Interrupt function;
			break;

		case 2: 	// Shutdown system
			call ISR shutdown System Interrupt function;
			set system shutdown status in a global variable to check in main and exit;
			break; 

		case 3: 	// Input operation completion – io_getc
			call ISR input Completion Interrupt function;
			break;

		case 4: 	// Output operation completion – io_putc
			call ISR output Completion Interrupt function;
			break;

		default:		// Invalid Interrupt ID
			Display invalid interrupt ID message;
			break;
	}  // end of switch InterruptID
	*/
	return;
}  // end of CheckAndProcessInterrupt() function

//Run Program Interrupt Service Routing
void ISRrunProgramInterrupt();
{
	/*
	Prompt and read filename;
	Call Create Process passing filename and Default Priority as arguments;
	*/
	return;
}  // end of ISRrunProgram() function

//Input Completion interrupt
Void ISRinputCompletionInterrupt()
{
	/*
	Prompt and read PID of the process completing input completion interrupt;

	Search WQ to find the PCB having the given PID
	{
Remove PCB from the WQ;
Read one character from standard input device keyboard;
Store the character in the GPR in the PCB;   // type cast char to long
Set process state to Ready in the PCB;
Insert PCB into RQ;
}

	If no match is found in WQ, then search RQ
	{
Read one character from standard input device keyboard;
Store the character in the GPR in the PCB;
	}

	If no matching PCB is found in WQ and RQ, print invalid pid as error message;
		*/
	return;
}  // end of ISRinputCompletionInterrupt() function

//Output Completion Interrupt
Void ISRoutputCompletionInterrupt()
{
	/*
	Prompt and read PID of the process completing input completion interrupt;

	Search WQ to find the PCB having the given PID
	{
Remove PCB from the WQ;
Print the character in the GPR in the PCB;
Set process state to Ready in the PCB;
Insert PCB into RQ;
}

	If no match is found in WQ, then search RQ
	{
Print the character in the GPR in the PCB;
	}

	If no matching PCB is found in WQ and RQ, print invalid pid as error message;

	*/
	return;

}  // end of ISRonputCompletionInterrupt() function

//Shutdown Sytem Interrupt
Void ISRshutdownSystem()
{
	/*
	// Terminate all processes in RQ one by one
	Set ptr = RQ;	// set ptr to first PCB pointed by RQ

	while(ptr is not equal to End Of List)
	{
		Set RQ = next PCB using ptr;
		Call Terminate Process passing ptr as argument;
		Set ptr = RQ;
	}

	// Terminate all processed in WQ one by one

	Code is similar to terminating processes in RQ given above.
	*/

	return;
}  // end of ISRshutdownSystem() function

//Search and Remove PCB from WQ
long SearchAndRemovePCBfromWQ ( long pid)
{
	/*
	long currentPCBptr = WQ;
	long previousPCBptr = EndOfList;

	// Search WQ for a PCB that has the given pid
	// If a match is found, remove it from WQ and return the PCB pointer
	while (currentPCBptr != EndOfList)
	{
		If(Memory[PCBptr + pidIndex] == pid)
		{
			// match found, remove from WQ
			if(previousPCBptr == EndOfList)
			{	// first PCB
				WQ = Memory[currentPCBptr + nextPCBIndex];
			}
			else
			{	// not first PCB
				Memory[previousPCBptr+nextPCBIndex] = 
						Memory[currentPCBptr+nextPCBIndex];
			}
			Memory[currentPCBptr+nextPCBIndex] = EndOfList;
			return(currentPCBptr);
		}
		previousPCBptr = currentPCBptr;
		currentPCBptr = Memory[currentPCBptr + nextPCBIndex];
	}  // end while currentPCBptr

	// No matching PCB is found, display pid message and return End of List code
	Display pid not found message;
	*/

	return (EndOfList);
}  // SearchAndRemovePCBfromWQ

//Revised Initialize System Function
void InitializeSystem()
{
	/*
	// Initialize all hardware component to zero: Main memory and CPU registers

Initialize all main memory locations to zero;	// Main memory

Initialize all GPRs to zero;	// General purpose registers

Set registers MAR, MBR, IR, PC, SP, PSR and Clock to zero;

// Create User free list using the free block address and size given in the class
Set User Free List = start address given in the class;
Set the next user free block pointer = End Of List;
Set second location in the free block = size of free block;  // size is given in the class

// Create OS free list using the free block address and size given in the class
Set OS Free List = start address given in the class;
Set next OS free block pointer = End Of List;
Set second location in the free block = size of free block;    // size is given in the class

Call Create Process function passing Null Process Executable File and priority zero as arguments
*/
	return;
}  // end InitializeSystem() function

































 


