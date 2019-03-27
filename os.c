// ************************************************************
// File: Simu.c
//
// Task Description:
// 	A simulator for Hypo machcine.
//
// Author
//	Fraz S. Ikram
//
// Date
//	Feb 2nd 2019
//
// Class
//	Operating System Internal
// Professor: Suban Krishnamoorthy
// ************************************************************
//First letinclude all the required system libraries.
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

//Let us define the Global constants.

#define MEMSIZE 10000
#define MAXCONTENT 999999
#define endOfProgram -1
#define INVALIDaddr -2
#define ErrorInvalidMode -3
#define ErrorInvalidOpcode -4
#define ErrorInvalidPCValue -5
#define ErrorFileOpen -6
#define ErrorInvalidAddress -7
#define ErrorInvalidGPR -8
#define ErrorStackoverflow -9
#define ErrorStackUnderflow -10
//In Unix, Okay is zero.
#define Okay 0
//HYPO Architecture
long MainMemory[MEMSIZE];//Main memory
long MAR; //Memory address register.
long MBR; //Memory buffer register.
long CLK=0; //System Clock.
long GPR[8]; //General purpose registers.
long IR; //Instruction register.
long PSR; //Process Status register
long PC; //Program counter
long SP; //Stack pointer
long ProgramSize;//program length

//Function prototypes
void InitializeSystem();		//System Initilizer
int AbsoluteLoader (char *);		//Program Loader
long FetchOperand(long , long, long *, long *); //Fetch an instruction from the memory.
long CPU(); //Executes the loaded program from the memory.
void DumpMemory(char*, long , long); //Dumps memory

// ******************************************************
// function: main
//
// Task Description:
//    The simulator flow to execute a Hypo program.
// Input parameters
//	none
// Output parameters
//	none
// Function return value
//	OK		- on successful execution
//	List all return error codes and their description here
// ******************************************************
int main ()
{



}  // end of main() function
// ************************************************************
//Function definitions.
// ********************************************************************
// Function: AbsoluteLoader
//
// Task Description:
// 	Open the file containing HYPO machine user program and 
//	load the content into HYPO memory.
//	On successful load, return the PC value in the End of Program line.
//	On failure, display appropriate error message and return appropriate error code
//
// Input Parameters
//	filename			Name of the Hypo Machine executable file
//
// Output Parameters
//	None
//
// Function Return Value will be one of the following:
//	ErrorFileOpen			Unable to open the file
//	ErrorInvalidAddress		Invalid address error
//	ErrorNoEndOfProgram	Missing end of program indicator
//	ErrorInvalidPCValue		invalid PC value
//	0 to Valid address range	Successful Load, valid PC value
// ************************************************************
int AbsoluteLoader ( char * filename)		// Input: file name of executable program
{

  ProgramSize=0;
  printf("Loader\n");
  
  //Open file
  FILE *filePtr= fopen(filename,"r");

  //Check if file is opened properly
  if(filePtr == NULL)
  {
    return (ErrorFileOpen);
  }

  int i=0;
  long address;
  long content;

  while(1)
  {
    //getting next address and content

    fscanf(filePtr,"%ld",&address);
    fscanf(filePtr,"%ld",&content);

    if(address== endOfProgram) // checking if the end of the program has been reached
    {
      if(content>=0 && content<MEMSIZE) //checking if it is within valid memory range
      {
        return content; //Program counter variable
      }
      else //invalid range
      {
          return(ErrorInvalidPCValue);
      }
    }

    else if (address>=0 && content<MAXCONTENT) // If it is not the end of the program
    {
      MainMemory[address]= content;
      ProgramSize++;
    }

    else //If address is invalid
    {
      return(ErrorInvalidAddress);
    }
  }

}  // end of AbsoluteLoader function



// ************************************************************
// Function: InitializeSystem
//
// Task Description:
// 	Set all global system hardware components to 0 
//
// Input Parameters
//	None
//
// Output Parameters
//	None
//
// Function Return Value
//	None
// ************************************************************

void InitializeSystem()
{
  printf("Initialization \n");
	// Initialize all Hypo Machine hardware components to zero
  /*
  (1)	Hypo memory array of size 10,000 locations (words)
    (2)	Hypo memory registers: (i) mar, (ii) mbr
    (3)	clock
    (4)	CPU registers: an array of 8 general purpose registers (gpr)
    (5)	CPU registers (i) IR, (ii) psr, (iii) pc, (iv) sp
  */

  MAR=0;
  MBR=0;
  CLK=0;
  IR=0;
  PSR=0;
  PC=0;
  SP=0;
  int a;
  for(a=0; a<MEMSIZE;a=a+1 )
  {
    //printf("%d\n", a);
    MainMemory[a]=0;
  }

  int b;

  for(b=0; b<8;b=b+1 )
  {
    GPR[b]=0;
  }



  return;
}  // end InitializeSystem() function



// ************************************************************
// Function: CPUexecuteProgram
//
// Task Description:
// 	 
//
// Input Parameters
//	None
//
// Output Parameters
//	None
//
// Function Return Value
//	List the error codes here
// ************************************************************

long CPU()
{
  printf(" In the CPU \n");
  CLK=0; // initialize clockto 0
  // Declare local variables as needed here
  long OpCode;
  long rem;//opcode remainder
  long Op1Mode;
  long Op1Gpr;
  long Op2Mode;
  long Op2Gpr;
  long Op1Address;
  long Op1Value;
  long Op2Address;
  long Op2Value;
  long stat =0;//for errors and current status
  long val;
   
  MBR= -2; //start the loop
  while(MBR!=0) 
  {//checking for errors and a valid Buffer
    //printf(" Inside while loop \n");
    //FETCH instruction.
    if(PC >=0 && PC<=MEMSIZE)
    {
      //Make Address register equal to program counter
      MAR = PC;
      //Increment Progran counter
      PC+=1;
      MBR=MainMemory[MAR]; //Read Hypo memory content pointed by MAR into MBR;

    }
    else
    {
      return -5;
    }

    IR=MBR; //Copy MBR value into instruction register IR;
    //DECODE
    //Get Opcode
    OpCode= IR/10000;
    rem= IR%10000;
    //Get O1pmode
    Op1Mode= rem/1000;
    rem= rem%1000;
    //Get op1 GPR
    Op1Gpr= rem/100;
    rem= rem%100;
    //get op2Mode
    Op2Mode= rem/10;
    rem= rem%10;
    //Op2 gpr
    Op2Gpr= rem;
    //printf("Opcode %ld\n", OpCode);

    // Error checking
    if(Op1Gpr<0 || Op1Gpr>7||Op2Gpr<0||Op1Gpr>7) //check if operand gpr are in valid range
    {
      return(ErrorInvalidGPR);
    }
    else if(Op1Mode<0||Op1Mode>6||Op2Mode<0||Op2Mode>6) // check if the mode is in vallid range
    {
      return(ErrorInvalidMode);
    }  
    else if(OpCode<0 || OpCode>12) //Check if opcode is valid
    {
      return(ErrorInvalidOpcode);
    }
    //No errors found, start execution cycle
    else
    {
      switch(OpCode)
      {
        case 0:
          //printf("Halted.\n");
          CLK=CLK+12;// halt encountered
          break;
          //ADD
         case 1: stat=  FetchOperand (Op1Mode, Op1Gpr,&Op1Address, &Op1Value);
        
            if(stat<0)
            {
              return (stat);
            }
            stat= FetchOperand(Op2Mode, Op2Gpr, &Op2Address, &Op2Value);
            if(stat<0)
            {
              return(stat);
            }
            //Add the operands
            Op1Value= Op1Value+Op2Value;
            if(Op1Mode==1) //Check if it is register mode
            {
              GPR[Op1Gpr]=Op1Value; //Store Op1value into the GPR
            }
            else if(Op1Mode==6)// ifit is immediate mode
            {
              return(ErrorInvalidMode);
            }
            else // for all other modes
            {
              MainMemory[Op1Address]=Op1Value; // store the value into main memory
            }
            CLK=CLK+3;
            break;
            //end of case 1
            //subtraction
            case 2: stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
              
              if(stat<0)
              {
                return(stat);
              }
              stat= FetchOperand(Op2Mode, Op2Gpr, &Op2Address, &Op2Value);
              if(stat<0)
              {
                return(stat);
              }

              Op1Value= Op1Value-Op2Value;
              //Register Mode
              if(Op1Mode==1)
              {
                GPR[Op1Gpr]= Op1Value; // put op1value into gpr
              }         

              else if (Op1Mode==6) //checking for immediate mode
              {
                return(ErrorInvalidMode); //invalid mode
              }

              //Other modes
              else
              {
                MainMemory[Op1Address]=Op1Value;
              }
              CLK=CLK+3;
              break;
              //end of case 2

              //Case:Multiply
               case 3: stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
             
              if(stat<0)
              {
                return(stat);
              }
              stat= FetchOperand(Op2Mode, Op2Gpr, &Op2Address, &Op2Value);
              if(stat<0)
              {
                return(stat);
              }

              Op1Value= Op1Value*Op2Value;
              //Register Mode
              if(Op1Mode==1)
              {
                GPR[Op1Gpr]= Op1Value; // put op1value into gpr
              }         

              else if (Op1Mode==6) //checking for immediate mode
              {
                return(ErrorInvalidMode); //invalid mode
              }

              //Other modes
              else
              {
                MainMemory[Op1Address]=Op1Value;
              }
              CLK=CLK+6;
              break;
              //End of case 3
              //Case 4 Divide
              case 4: stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
              
              if(stat<0)
              {
                return(stat);
              }
              stat= FetchOperand(Op2Mode, Op2Gpr, &Op2Address, &Op2Value);
              if(stat<0)
              {
                return(stat);
              }

              Op1Value= Op1Value/Op2Value;
              //Register Mode
              if(Op1Mode==1)
              {
                GPR[Op1Gpr]= Op1Value; // put op1value into gpr
              }         

              else if (Op1Mode==6) //checking for immediate mode
              {
                return(ErrorInvalidMode); //invalid mode
              }

              //Other modes
              else
              {
                MainMemory[Op1Address]=Op1Value;
              }
              CLK=CLK+6;
              break;
              //End of case 4
              //Move
              case 5: stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);

              if(stat<0)
              {
                return(stat);
              }
              stat= FetchOperand(Op2Mode, Op2Gpr, &Op2Address, &Op2Value);
              if(stat<0)
              {
                return(stat);
              }

              Op1Value= Op2Value;
              //Register Mode
              if(Op1Mode==1)
              {
                GPR[Op1Gpr]= Op1Value; // put op1value into gpr
              }         

              else if (Op1Mode==6) //checking for immediate mode
              {
                return(ErrorInvalidMode); //invalid mode
              }

              //Other modes
              else
              {
                MainMemory[Op1Address]=Op1Value;
              }
              CLK=CLK+2;
              break;
              //End of Case 5
              //Case 6 Branch
              case 6:
               
                if(PC<MEMSIZE && PC>=0) //Checking PC range
                {
                  PC = MainMemory[(int)PC];
                }
                else
                {
                  return ErrorInvalidPCValue;
                }
                CLK=CLK+2;
                break; 
                //End of case 6
                //Case 7 Branch on Minus
              case 7: stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
               
                if(stat<0)
                {
                  return(stat);
                }
                if(Op1Value<0)
                {
                  if(PC<MEMSIZE && PC>=0)
                  {
                    PC=MainMemory[PC];
                  }
                  else
                  {
                    return ErrorInvalidPCValue;
                  }
                }
                //If opcode is greater than or equal to 0
                else
                {
                  PC++;
                }
                CLK=CLK+4;
                break;
                //end of case 7
                //Branh on plus
                case 8: stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
                
                if(stat<0)
                {
                  return stat;
                }
                if(Op1Value>0)
                {
                  if(PC<MEMSIZE && PC>=0)
                  {
                    PC=MainMemory[PC];
                  }
                  else
                  {
                    return ErrorInvalidPCValue;
                  }
                }
                else
                {
                  PC++;
                }
                 CLK=CLK+4;
                break;
                //end of case 8
                //Branch on zero
                
                case 9:
                 
                stat = FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
                if(stat<0)
                {
                  return stat;
                }
                if(Op1Value==0)
                {
                  if(PC<MEMSIZE && PC>=0)
                  {
                    PC=MainMemory[PC];
                  }
                  else
                  {
                    return ErrorInvalidPCValue;
                  }
                }
                else
                {
                  PC++;
                }
                 CLK=CLK+4;
                break;
                //end of case 9
                //Case 10 Push
                case 10: 
                 
                stat= FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
                if(stat<0)
                {
                  return stat;
                }
                if(MEMSIZE<SP)
                {
                  return ErrorStackoverflow;
                }
                  SP++;
                  MainMemory[SP]=Op1Value;
                  printf("%ld Has been added to the Stack\n",Op1Value );
                  break;
                   CLK=CLK+2;
                  //End of case 10
                  //Case 11 pop
                case 11:  
                
                stat= FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
                if(stat<0)
                {
                  return stat;
                }
                if (SP<0)
                {
                  return ErrorStackUnderflow;
                }
                if(SP<= MEMSIZE && SP>0)
                {
                  Op1Address= MainMemory[SP];
                  SP--;
                }
                printf("%ld got popped\n",Op1Address);
                 CLK=CLK+2;

                break;
                //case 12 system call
                case 12: 
               
                stat= FetchOperand(Op1Mode,Op1Gpr, &Op1Address,&Op1Value);
                if(stat<0)
                {
                  return stat;
                }
                break;

                //for invalid opcode
            default: return ErrorInvalidOpcode;

    }




  }
}
return stat;

}  // end of CPUexecuteProgram() function



// ************************************************************
// Function: FetchOperand
//
// Task Description:
// 	
//
// Input Parameters
//	OpMode			Operand mode value
//	OpReg				Operand GPR value
// Output Parameters
//	OpAddress			Address of operand
//	OpValue			Operand value when mode and GPR are valid
//
// Function Return Value
//	Okay				On successful fetch
//	List all possible error codes here
// ************************************************************

long FetchOperand(long  OpMode, 	long  OpReg,	long  *OpAddress,long  *OpValue)	// Operand value, output parameter
{
	switch(OpMode)
  {
    //Register Mode
    case 1: 
    *OpAddress = -2;//OpAdress set to an invalid value
    *OpValue=GPR[OpReg]; //OPValue set to gpr
    break;

    //Register Deferred mode
    case 2: 
    *OpAddress= GPR[OpReg];
    *OpValue= GPR[OpReg];
    break;
    case 3: //Autoincrement mode 
    *OpAddress= GPR[OpReg];
    if(*OpAddress>0)
    {
      *OpValue= MainMemory[*OpAddress];
    }
    else
    {
      return(ErrorInvalidAddress);
    }
    GPR[OpReg]++;
    break;

    //Autodecrement mode
    case 4: --GPR[OpReg];
    *OpAddress = GPR[OpReg];
    if(*OpAddress>0)
    {
      *OpValue=MainMemory[*OpAddress];
    }
    else
    {
      return(ErrorInvalidAddress);
    }
    break;
    //Direct Mode
    case 5: *OpAddress=MainMemory[PC];
      PC++;
    if(*OpAddress>=0)
    {
      *OpValue=MainMemory[*OpAddress];
    }
    else
    {
      return(ErrorInvalidAddress);
    }
    break;

  //Immediate Mode
  case 6:// Instruction contains operand value
  if(PC<MEMSIZE)
  {
    *OpValue=MainMemory[PC];
    PC+=1;
    break;
  }
  else
  {
    return ErrorInvalidPCValue;
  }
  default: return(ErrorInvalidMode);
}
return 1;
    // return success status
}  // end of FetchOperand() function

// ************************************************************
// Function: DumpMemory
//
// Task Description:
//	Displays a string passed as one of the  input parameter.
// 	Displays content of GPRs, SP, PC, PSR, system Clock and
//	the content of specified memory locations in a specific format.
//
// Input Parameters
//	String				String to be displayed
//	StartAddress			Start address of memory location
//	Size				Number of locations to dump
// Output Parameters
//	None			
//
// Function Return Value
//	None				
// ************************************************************
void DumpMemory(char *String, long StartAddress, long size)
{
  //Display input parameter String;
  printf("%s\n",String);

  printf("GPRs:\tG0\tG1\tG2\tG3\tG4\tG5\tG6\tG7\tSP\tPC");
  printf("\n");
  printf("\t");
  for(int i=0; i<8;i++)
  {
    printf("%ld\t",GPR[i]);
  }

  printf("%ld\t",SP);
  printf("%ld\t",PC);
  printf("\n");
  printf("Address:+0\t+1\t+2\t+3\t+4\t+5\t+6\t+7\t+8\t+9\n");
  long addr= StartAddress;
  long endAddress= StartAddress+size;
  while(addr<endAddress)
  {
    // printf("\t");
    printf("%ld\t",addr );

    for(int i =0; i<10; i++)
    {
      if(addr<endAddress)
      {
        printf("%ld\t",MainMemory[addr]);
        addr++;
      }
      else
      {
        break;
      } // end of 'for i' loop
      
    }
    printf("\n");
  } // end of while loop
  printf("\n");
  printf("Clock =%ld\n",CLK );//Display clock
  printf("PSR =%ld\n",PSR );// display PSR value
  return;
}  // end of DumpMemory() function

//System Call
long SystemCall(long SystemCallID)
{
  /*
  Set PSR to OS Mode; // Set system mode to OS mode

  Declare long status set to OK;

  switch(SystemCallID)
  {
    Case 1: // Create process – user process is creating a child process. 
Display create process system call not implemented;
      break;

    Case 2:  // Delete process
Display delete process system call not implemented;
      break;

    Case 3:  // process inquiry
Display process inquiry system call not implemented;
      break;

    Case 4:  // Dynamic memory allocation: Allocate user free memory system call
Status = MemAllocSystemCall();
      break;

    Case 5:  // Free dynamically allocated user memory system call
Status = MemFreeSystemCall();
      break;


    // Display system call not implemented message 
// for all other system calls like above


    Case 8:  // io_getc  system call
Status = io_getcSystemCall ();
      break;

    Case 9:  // io_putc system call
Status = io_putcSystemCall();
      break;


    // Display system call not implemented message 
// for all other system calls like above


    Default:    // Invalid system call ID
      Display invalid system call ID error message;
      break;
  }  // end of SystemCallID switch statement

  Set PSR to User Mode; // Set system mode to user mode
*/
  return (status);
}  // end of SystemCall() function

//Allocate Dynamic User Memory System Call

long MemAllocSystemCall()
{
  /*
  // Allocate memory from user free list
  // Return status from the function is either the address of allocated memory or an error code

  Declare long Size and set it to GPR2 value;

  // Add code here to check for size out of range

  // Check size of 1 and change it to 2
  If (Size  == 1)
    Size = 2;

  Set GPR1 = Allocate User Memory passing Size argument;
  if(GPR1 is less than 0)
  {
    Set GPR0 = GPR1;  // Set GPR0 to have the return status
  }
  else
  {
    Set GPR0 to OK;
  }

              Display Mem_alloc system call, and parameters GPR0, GPR1, GPR2
*/

  Return GPR0;
}  // end of MemAllocaSystemCall() function

//Free Dynamic User Memory System Call

long MemFreeSystemCall()
{
  /*
  // Return dynamically allocated memory to the user free list
  // GPR1 has memory address and GPR2 has memory size to be released
  // Return status in GPR0

  Declare long Size setting it to GPR2 value;

  // Add code to check for size out of range

  // Check size of 1 and change it to 2
  If (Size is 1)
    Set Size to 2;

  Set GPR0 = Call Free User Memory(pass GPR1 and Size as arguments);

              Display Mem_free system call, and parameters GPR0, GPR1, GPR2;

  */
  Return GPR0;
}  // end of MemAllocaSystemCall() function

//io_getc System Call

long io_getcSystemCall ()
{
  //Return start of input operation event code;

}  // end of io_getc system call

long CreateProcess (char &filename, long priority)  // or char * pointer
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
       if (ptr < 0) // check for error
      {  // User memory allocation failed
  Free allocated PCB space by calling Free OS Memory passing PCBptr and  PCBsize;
  return(ptr);  // return error code
      }

      // Store stack information in the PCB – SP, ptr, and size
      Set SP in the PCB = ptr + Stack Size;  // empty stack is high address, full is low address
      Set stack start address in the PCB to ptr;
      Set stack size in the PCB = Stack Size;

      Set priority in the PCB = priority; // Set priority

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
     /* Set entire PCB area to 0 using PCBptr;  // Array initialization
     
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

     Memory[PCBptr + StateIndex] = Ready;   // set state to ready state
     Memory[PCBptr + NextPointerIndex] = EndOfList;  // set next pointer to end of list

     if( RQ == EndOfList) // RQ is empty
     {
  RQ = PCBptr;
  return(OK);
     }

  // Walk thru RQ and find the place to insert
  // PCB will be inserted at the end of its priority

     while( currentPtr != EndOfList)
     {
if(Memory[PCBptr + PriorityIndex] > Memory[currentPtr + PriorityIndex])
{ // found the place to insert
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
{ // go to the next PCB in RQ
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

Memory[PCBptr + StateIndex] = Waiting;  // set state to ready state
Memory[PCBptr + NextPointerIndex] = WQ;  // set next pointer to end of list

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

  Set SP field in the PCB = SP;   // Save SP
  Set PC field in the PCB = PC; // Save PC
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
  //PSR = UserMode; // UserMode is 2, OSMode is 1.

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
    Return(CurrentPtr); // return memory address
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
    return(CurrentPtr); // return memory address
        }
        else  // not first black
        {
    Memory[CurrentPtr + RequestedSize] = Memory[CurrentPtr];  // move next block ptr
    Memory[CurrentPtr + RequestSize + 1] = Memory[CurrentPtr +1] – RequestedSize;
    Memory[PreviousPtr] = CurrentPtr + RequestedSize;  // address of reduced block
    Memory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
    return(CurrentPtr); // return memory address
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
     if (ptr is outside the OS free list area)    // Address range is given in the class
     {
  display invalid address error message;
  return(ErrorInvalidMemoryAddress);  // ErrorInvalidMemoryAddress is constantset to  < 0
     }
    
     if(size is 1)  // check for minimum allocated size, which is 2 even if user asks for 1 location
     {
  Set size = 2;  // minimum allocated size
      }
     else if( size is less than 1) OR ((ptr+size) greater than or equal to Maximum Memory Address))
     {  // Invalid size
  display invalid size or address error message;
  return(ErrorInvalidSizeORMemoryAddress);    // All error codes are < 0
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
     if(size is 1)  // check for minimum allocated size, which is 2 even if user asks for 1 location
     {
  Set size = 2;  // minimum allocated size
      }
     else if((size < 1) OR ((ptr+size) outside maximum free user memory area))  // invalid size
     {  // Invalid size
  display invalid size or address error message;
  return(ErrorInvalidSizeORMemoryAddress);    // All error codes are < 0
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
  Display possible interrupts:  // 0 – no interrupt
          // 1 – run program
          // 2 – shutdown system
          // 3 – Input operation completion (io_getc)
          // 4 – Output operation completion (io_putc)
  Read interrupt ID;
              Display the interrupt value that was read;
  
  // Process interrupt
  switch(InterruptID)
  {
    case 0:   // No interrupt
      break;

    case 1:   // Run program
      call ISR run Program Interrupt function;
      break;

    case 2:   // Shutdown system
      call ISR shutdown System Interrupt function;
      set system shutdown status in a global variable to check in main and exit;
      break; 

    case 3:   // Input operation completion – io_getc
      call ISR input Completion Interrupt function;
      break;

    case 4:   // Output operation completion – io_putc
      call ISR output Completion Interrupt function;
      break;

    default:    // Invalid Interrupt ID
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
  Set ptr = RQ; // set ptr to first PCB pointed by RQ

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
      { // first PCB
        WQ = Memory[currentPCBptr + nextPCBIndex];
      }
      else
      { // not first PCB
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

Initialize all main memory locations to zero; // Main memory

Initialize all GPRs to zero;  // General purpose registers

Set registers MAR, MBR, IR, PC, SP, PSR and Clock to zero;*/


  MAR=0;
  MBR=0;
  CLK=0;
  IR=0;
  PSR=0;
  PC=0;
  SP=0;
  int a;
  for(a=0; a<MEMSIZE;a=a+1 )
  {
    //printf("%d\n", a);
    MainMemory[a]=0;
  }
   int b;

  for(b=0; b<8;b=b+1 )
  {
    GPR[b]=0;
  }

  /*
// Create User free list using the free block address and size given in the class
Set User Free List = start address given in the class;
Set the next user free block pointer = End Of List;
Set second location in the free block = size of free block;  // size is given in the class*/





/*// Create OS free list using the free block address and size given in the class
Set OS Free List = start address given in the class;
Set next OS free block pointer = End Of List;
Set second location in the free block = size of free block;    // size is given in the class

Call Create Process function passing Null Process Executable File and priority zero as arguments
*/
  return;
}  // end InitializeSystem() function

































 






