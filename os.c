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

//Return Codes
#define Okay 0 //In Unix, Okay is zero.
#define endOfProgram -1
#define EndOfList -1
#define INVALIDaddr -2
#define ErrorInvalidMode -3
#define ErrorInvalidOpcode -4
#define ErrorInvalidPCValue -5
#define ErrorFileOpen -6
#define ErrorInvalidAddress -7
#define ErrorInvalidGPR -8
#define ErrorStackoverflow -9
#define ErrorStackUnderflow -10
#define ErrorNoEndOfProgram -11
#define ErrorNoFreeMemory -12
#define ErrorRunTime -13
#define ErrorInvalidMemorySize -13
#define ErrorInvalidSizeORMemoryAddress -14
#define Halt 1
#define TimeSliceExpired 2
#define InputOp 3
#define OutputOp 4
//Global Constants
#define MEMSIZE 10000
#define MAXCONTENT 999999
#define StackSize 10
#define TimeSlice 200
#define OSMode 1
#define UserMode 2
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
long RQ; //Ready Queue
long WQ; //Waiting Queue
long UserFreeList; //User Free List
long OSFreeList;// OS free list
long ProcessID; //Process ID
int shutdown; //Shutdown

//Function prototypes
void InitializeSystem();		//System Initilizer
int AbsoluteLoader (char *);		//Program Loader
long FetchOperand(long , long, long *, long *); //Fetch an instruction from the memory.
long CPU(); //Executes the loaded program from the memory.
void DumpMemory(char*, long , long); //Dumps memory
long AllocateOSMemory(long RequestedSize);
long CreateProcess(char* filename, long priority);
void InitializePCB(long PCBptr);
void PrintPCB(long PCBptr);
long PrintQueue(long Qptr);
long InsertIntoRQ(long PCBptr);
long InsertIntoWQ(long PCBptr);
long SelectProcessFromRQ();
void SaveContext(long PCBptr);
void Dispatcher(long PCBptr);
void TerminateProcess(long PCBptr);
long FreeOSMemory(long ptr,long size);
long AllocateUserMemory(long RequestedSize);
long FreeUserMemory(long ptr, long size);
void CheckAndProcessInterrupt();
long io_getcSystemCall();
long io_putcSystemCall();
long MemFreeSystemCall();
long MemAllocSystemCall();
long SystemCall(long systemCallID);
long SearchAndRemovePCBfromWQ ( long pid);
void ISRinputCompletionInterrupt();
void ISRoutputCompletionInterrupt();
void ISRshutdownSystem();

//PCB Components
const int DefaultPriority = 128;
const int ReadyState = 1;
const int Running = 2;
const int Waiting = 3;
const int PCBsize = 22;
const int NextPointer =0;
const int pcbPID = 1;
const int PCBState =2;
const int PCBReason = 3;
const int PCBPriority = 4;
const int pcbStackStartAddr = 5;
const int pcbStackSize = 6;
const int pcbGPR0 = 11;
const int pcbGPR1 = 12;
const int pcbGPR2 = 13;
const int pcbGPR3 = 14;
const int pcbGPR4 = 15;
const int pcbGPR5 = 16;
const int pcbGPR6 = 17;
const int pcbGPR7 = 18;
const int pcbSP = 19;
const int pcbPC = 20;
const int pcbPSR = 21;



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


  int status;
  //Call Initialize System function;
  InitializeSystem();

  // Run until shutdown
  while (!shutdown)
  {
    // Check and process interrupt
    //call Check and Process Interrupt function and store return status
    CheckAndProcessInterrupt();
    //if interrupt is shutdown, exit from main;
    if(shutdown)
    {
      printf("%s\n","Shutting Down");
      return status;
    }

    // Dump RQ and WQ
    //DumpRQ(“RQ: Before CPU scheduling\n”);
    printf("%s\n","RQ before scheduling" );
    PrintQueue(RQ); 
     //DumpWQ(“WQ: Before CPU scheduling\n”);
    printf("%s\n","WQ before scheduling" );
    PrintQueue(WQ);
     //DumpMemory(“Dynamic Memory Area before CPU scheduling\n”);
    DumpMemory("Dynamic Area before CPU scheduling\n",0,250);

    // Select next process from RQ to give CPU
    //Running PCB ptr = Select Process From RQ (); 
     // call the function
    long PCBptr=SelectProcessFromRQ();
 // Perform restore context using Dispatcher
   // Call Dispatcher function with Running PCB ptr as argument;
    Dispatcher(PCBptr);
  //Dump RQ(“RQ: After selecting process from RQ\n”);
    PrintQueue(RQ);
    //Dump Running PCB and CPU Context passing  Running PCB ptr as argument;
    PrintPCB(PCBptr);
    SaveContext(PCBptr);
    // Execute instructions of the running process using the CPU
    //status = CPU Execute Program ();  
    // call the function
    status=CPU();

    // Dump dynamic memory area
    //Dump dynamic memory calling Dump Memory function(“After execute program\n”);
    DumpMemory("Dynamic Area after CPU scheduling\n",0,250);

    // Check return status – reason for giving up CPU
    if(status == TimeSliceExpired)
    {
      printf("%s\n","Time Slice Expired" );
      //Save CPU Context of running process in its PCB;  // running process is losing CPU
      SaveContext(PCBptr);
      //Insert running process PCB into RQ;
      InsertIntoRQ(PCBptr);
      //Set Running PCB ptr = End Of List;
      PCBptr=EndOfList;
    }
    else if((status == Halt) || (status < 0))  // Halt or run-time error
    {
      //Terminate running Process;
      TerminateProcess(PCBptr);
      //Set Running PCB ptr = End Of List ;
      PCBptr=EndOfList;
    }
    else if(status == InputOp)   // io_getc
    {
      printf("%s\n","Input Operation" );
      //Set reason for waiting in the running PCB to Input Completion Event;
     SaveContext(PCBptr);
     // Insert running process into WQ;
     InsertIntoWQ(PCBptr);
    //Set Running PCB ptr = End Of List;
     PCBptr= EndOfList;
    }
    else if(status == OutputOp)    // io_putc
    {
      printf("%s\n","Output Operation" );
      //Set reason for waiting in the running PCB to Input Completion Event;
       SaveContext(PCBptr);
      //Insert running process into WQ;
        InsertIntoWQ(PCBptr);
      //Set Running PCB ptr = End Of List;
        PCBptr= EndOfList;
    }
    else
    { // Unknown programming error
     // Display Unknown programming error message;
       printf("%s\n","Unknown programming error" );
     }
  }  // end of while not shutdown loop

  //Print OS is shutting down message;
  printf("%s\n","OS is shutting down" );

  return(status);  // Terminate Operating System


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
int AbsoluteLoader ( char * filename)		//Method by Fraz Ikram
// Input: file name of executable program
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
      if(content>=0 && content<2500) //checking if it is within valid memory range
      {
        return content; //Program counter variable
      }
      else //invalid range
      {
          return(ErrorInvalidPCValue);
      }
    }

    else if (address>=0 && content<2500) // If it is not the end of the program
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

void InitializeSystem() //Method by Fraz Ikram
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

  for(b=0; b<8;b++)
  {
    GPR[b]=0;
  }
  UserFreeList= 2500;
  MainMemory[UserFreeList]=EndOfList;
  MainMemory[UserFreeList +1]=5000;

  OSFreeList=7500;
  MainMemory[OSFreeList]=EndOfList;
  MainMemory[OSFreeList+1]=2500;
  RQ=EndOfList;
  WQ=EndOfList;


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

long CPU() //Method by Fraz Ikram
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
  long TimeLeft=TimeSlice;
   
  MBR= -2; //start the loop
  while(TimeLeft>0) 
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
      return (ErrorInvalidAddress);
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
          TimeLeft=TimeLeft-12;
          return(Halt);
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
                stat=SystemCall(Op1Value);
                CLK=CLK+12;
                TimeLeft=TimeLeft-12;
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

long FetchOperand(long  OpMode, 	long  OpReg,	long  *OpAddress,long  *OpValue)	//Method by Fraz Ikram
// Operand value, output parameter
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
void DumpMemory(char *String, long StartAddress, long size) //Method by Fraz Ikram
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
long SystemCall(long SystemCallID)//Method by Connor Huggan
{
  PSR = OSMode;
  long status = Okay;

  switch(SystemCallID)
  {
    case 1: printf("Create process system call was not implemented\n");
    break;
    case 2: printf("Delete process system call was not implemented\n");
    break;
    case 3: printf("Process inquiry system call was not implemented\n");
    break;
    case 4: status = MemAllocSystemCall();
    break;
    case 5: status = MemFreeSystemCall();
    break;
    case 6: break;
    case 7: break;
    case 8: status = io_getcSystemCall();
    break;
    case 9: status = io_putcSystemCall();
    break;
    default: printf("Invalid system call ID\n");
    break;

    return(status);
  }

  return (status);
}  // end of SystemCall() function

//Allocate Dynamic User Memory System Call

long MemAllocSystemCall()//Method by Connor Huggan
{
  long size = GPR[2];

  GPR[1] = AllocateUserMemory(size);
  if(GPR[1] < 0)
  {
    GPR[0] = GPR[1];
  }
  else
  {
    GPR[0] = Okay;
  }
  printf("Memory Aollocation System Call \n%ld\n%ld\n%ld\n", GPR[0], GPR[1], GPR[2]);
  return(GPR[0]);
}
//Free Dynamic User Memory System Call

long MemFreeSystemCall()//Method by Connor Huggan
{
long size = GPR[2];

  GPR[0] = FreeUserMemory(GPR[1], size);
  printf("Memory Free System Call \n%ld\n%ld\n%ld\n",GPR[0], GPR[1], GPR[2]);
  return(GPR[0]);
}  // end of MemAllocaSystemCall() function

//io_getc System Call

long io_getcSystemCall ()
{
  //Return start of input operation event code;
  printf("get_c system call\n");
  return(InputOp);

}  // end of io_getc system call
long io_putcSystemCall ()
{
  //Return start of output operation event code;
  printf("put_c system call\n");
  return(OutputOp);

}  // end of io_putc system call


long CreateProcess (char* filename, long priority)  // or char * pointer
{
  
  // Allocate space for Process Control Block
      long PCBptr= AllocateOSMemory(PCBsize);
       // return value contains address or error
      if(PCBptr<0)
      {
        return(PCBptr);
      }

      // Initialize PCB: Set nextPCBlink to end of list, default priority, Ready state, and PID
      //Initialize PCB passing PCBptr as argument;
      InitializePCB(PCBptr);
 
      // Load the program
      //Set value =load the program calling by Absolute Loader passing filename as argument;
      long value= AbsoluteLoader(filename);

      //Check for error and return error code, if loading program failed

      if(value<0)
      {
        return (value);
      }

      // store PC value in the PCB of the process
     //Set PC value in the PCB = value; 
     MainMemory[PCBptr+pcbPC]=value; 

      // Allocate stack space from user free list
      //Set ptr = Allocate User Memory of size StackSize;
     long ptr= AllocateUserMemory(StackSize);
       if (ptr < 0) // check for error
      {  // User memory allocation failed
        // Free allocated PCB space by calling Free OS Memory passing PCBptr and  PCBsize;
        FreeOSMemory(PCBptr,PCBsize);
        return(ptr);  // return error code
      }

      // Store stack information in the PCB – SP, ptr, and size
      //Set SP in the PCB = ptr + Stack Size;  
      MainMemory[PCBptr+pcbSP]=ptr+StackSize;
      // empty stack is high address, full is low address
      //Set stack start address in the PCB to ptr;
      MainMemory[PCBptr+pcbStackStartAddr]=ptr;
      //Set stack size in the PCB = Stack Size;
      MainMemory[PCBptr+pcbStackSize]=StackSize;
    // Set priority in the PCB = priority; // Set priority
      MainMemory[PCBptr+PCBPriority]=priority;

     //Dump program area;
      DumpMemory("Dumping program area.\n",0,200);

     //Print PCB passing PCBptr; 
     PrintPCB(PCBptr);

     // Insert PCB into Ready Queue according to the scheduling algorithm
     //Insert PCB into Ready Queue passing PCBptr;
     InsertIntoRQ(PCBptr);
  
     return(Okay);
}  // end of CreateProcess() function



void InitializePCB (long PCBptr)// Method by Fraz Ikram
{
	//Initialize PCB
	for(int i=0; i<PCBsize; i++)  //Set entire PCB area to 0 using PCBptr;  
	{
		MainMemory[PCBptr+i]=0; // Array initialization
	}
	MainMemory[PCBptr+pcbPID]= ProcessID++;   
	// Allocate PID and set it in the PCB. PID zero is invalid
   //  Set PID field in the PCB to = ProcessID++;  
   // ProcessID is global variable initialized to 1
	MainMemory[PCBptr+PCBPriority]=DefaultPriority;
	// Set priority field in the PCB = Default Priority;  
	// DefaultPriority is a constant set to 128
	MainMemory[PCBptr+PCBState]= ReadyState;
	//Set state field in the PCB = ReadyState;    
	// ReadyState is a constant set to 1
	MainMemory[PCBptr+NextPointer]= EndOfList;
	 // Set next PCB pointer field in the PCB = EndOfList;  
	 // EndOfList is a constant set to -1
	return;
}  // end of InitializePCB

//Print PCB values

void PrintPCB(long PCBptr)//Method by Fraz Ikram
{
  //Print the values of the following fields from PCB with a text before the value like below:
  //PCB address = 6000, Next PCB Ptr = 5000, PID = 2, State = 2, PC = 200, SP = 4000, 
    printf("PCB Address = %ld\n", PCBptr);
    printf("Next PCB Ptr = %ld\n", MainMemory[PCBptr + NextPointer]);
    printf("PID = %ld\n", MainMemory[PCBptr + pcbPID]);
    printf("State = %ld\n", MainMemory[PCBptr + PCBState]);
    printf("PC = %ld\n", MainMemory[PCBptr + pcbPC]);
    printf("SP = %ld\n", MainMemory[PCBptr + pcbSP]);
  //Priority = 127, Stack info: start address = 3990, size = 10
    printf("Priority = %ld\n", MainMemory[PCBptr + PCBPriority]);
    printf("Stack Info: Start Address = %ld\n", MainMemory[PCBptr + pcbStackStartAddr]);
    
  //GPRs = print 8 values of GPR 0 to GPR 7
    printf("Size = %ld\n", MainMemory[PCBptr + pcbStackSize]);
    printf("GPR0 = %ld\n", MainMemory[PCBptr + pcbGPR0]);
    printf("GPR1 = %ld\n", MainMemory[PCBptr + pcbGPR1]);
    printf("GPR2 = %ld\n", MainMemory[PCBptr + pcbGPR2]);
    printf("GPR3 = %ld\n", MainMemory[PCBptr + pcbGPR3]);
    printf("GPR4 = %ld\n", MainMemory[PCBptr + pcbGPR4]);
    printf("GPR5 = %ld\n", MainMemory[PCBptr + pcbGPR5]);
    printf("GPR6 = %ld\n", MainMemory[PCBptr + pcbGPR6]);
    printf("GPR7 = %ld\n", MainMemory[PCBptr + pcbGPR7]);
    
}  // end of PrintPCB() function

//Print Given Queue

long PrintQueue (long Qptr) //Method by Fraz Ikram
{
  // Walk thru the queue from the given pointer until end of list
  // Print each PCB as you move from one PCB to the next

   long currentPCBPtr = Qptr;

  if(currentPCBPtr==EndOfList)
  {
    printf("Empty list.\n");
    return(Okay);
  }

  // Walk thru the queue
  while(currentPCBPtr != EndOfList)
  {
    //Print PCB passing currentPCBPtr;
    PrintPCB(currentPCBPtr);
    currentPCBPtr = MainMemory[currentPCBPtr+NextPointer];
  }  // end of while loop

  return (Okay);
}  // end of PrintQueue() function


//Insert PCB into ready Queue

long InsertIntoRQ (long PCBptr)//Method by Fraz Ikram
{
  
  // Insert PCB according to Priority Round Robin algorithm
  // Use priority in the PCB to find the correct place to insert.
	long previousPtr = EndOfList;
	long currentPtr = RQ;

  // Check for invalid PCB memory address
     if((PCBptr < 0) || (PCBptr >MEMSIZE-1))
     {
  //display invalid address error message;
     	printf("Error: PCB address is invalid. \n");
 		 return(ErrorInvalidAddress);  // Error code < 0
     }

     MainMemory[PCBptr + PCBState] = ReadyState;   // set state to ready state
     MainMemory[PCBptr + NextPointer] = EndOfList;  // set next pointer to end of list

     if( RQ == EndOfList) // RQ is empty
     {
 		 RQ = PCBptr;
  		return(Okay);
     }

  // Walk thru RQ and find the place to insert
  // PCB will be inserted at the end of its priority

     while( currentPtr != EndOfList)
     {
		if(MainMemory[PCBptr + PCBPriority] > MainMemory[currentPtr + PCBPriority])
		{ // found the place to insert
       	if(previousPtr == EndOfList)
       	{
  		// Enter PCB in the front of the list as first entry
  		MainMemory[PCBptr + NextPointer] = RQ;
  		RQ = PCBptr;
  		return(Okay);
       }
  // enter PCB in the middle of the list
       MainMemory[PCBptr +NextPointer] = MainMemory[previousPtr + NextPointer];
       MainMemory[previousPtr + NextPointer] = PCBptr;
       return(Okay);
}
else  // PCB to be inserted has lower or equal priority to the current PCB in RQ
{ // go to the next PCB in RQ
  previousPtr = currentPtr;
  currentPtr = MainMemory[currentPtr+NextPointer];
}
     }  // end of while loop

     // Insert PCB at the end of the RQ
     MainMemory[previousPtr + NextPointer] = PCBptr;
     
     return(Okay); 

}  // end of InsertIntoRQ() function

//Insert PCB into waiting queue

long InsertIntoWQ (long PCBptr) //Method by Fraz Ikram
{ 
  // Insert the given PCB at the front of WQ
   // Check for invalid PCB memory address
  if((PCBptr < 0) || (PCBptr >MEMSIZE))
    {
      printf("Error: Invalid PCB address.\n");
      return(ErrorInvalidAddress);  // Error code < 0
    }
    MainMemory[PCBptr + PCBState] = Waiting;  // set state to ready state
      MainMemory[PCBptr + NextPointer] = WQ;  // set next pointer to end of list

      WQ = PCBptr;

      return (Okay);
}    
 // end of InsertIntoWQ () function

//Select first process from RQ to Give CPU
long SelectProcessFromRQ()//Method by Fraz Ikram
{
  
  //Declare PCBptr as type long and initialize to RQ;  // first entry in RQ
  long PCBptr=RQ;

  if(RQ != EndOfList)
  {
        // Remove first PCB from RQ
       RQ = MainMemory[RQ+NextPointer];
  }

  // Set next point to EOL in the PCB
  //Set Next PCB field in the given PCB to End Of List;
  MainMemory[PCBptr+NextPointer]=EndOfList;
            

  return(PCBptr);
}  // end of SelectProcessFromRQ() function

//Save CPU context into Running Process PCB
void SaveContext(long PCBptr) //Fraz Ikram
{
  // Assume PCBptr is a valid pointer.
  

  //Copy all CPU GPRs into PCB using PCBptr with or without using loop
	MainMemory[PCBptr+pcbGPR0]=GPR[0];
	MainMemory[PCBptr+pcbGPR1]=GPR[1];
	MainMemory[PCBptr+pcbGPR2]=GPR[2];
	MainMemory[PCBptr+pcbGPR3]=GPR[3];
	MainMemory[PCBptr+pcbGPR4]=GPR[4];
	MainMemory[PCBptr+pcbGPR5]=GPR[5];
	MainMemory[PCBptr+pcbGPR6]=GPR[6];
	MainMemory[PCBptr+pcbGPR7]=GPR[7];

  //Set SP field in the PCB = SP;  Save SP
	MainMemory[PCBptr+pcbSP]=SP;
  //Set PC field in the PCB = PC Save PC
	MainMemory[PCBptr+pcbPC]=PC;
  
  return;
}  // end of SaveContext() function

//Restore CPU context from the given PCB
void Dispatcher(long PCBptr)//Method by Fraz Ikram
{
  // PCBptr is assumed to be correct.

  // Copy CPU GPR register values from given PCB into the CPU registers
  // This is opposite of save CPU context
  GPR[0]=MainMemory[PCBptr+pcbGPR0];
  GPR[1]=MainMemory[PCBptr+pcbGPR1];
  GPR[2]=MainMemory[PCBptr+pcbGPR2];
  GPR[3]=MainMemory[PCBptr+pcbGPR3];
  GPR[4]=MainMemory[PCBptr+pcbGPR4];
  GPR[5]=MainMemory[PCBptr+pcbGPR5];
  GPR[6]=MainMemory[PCBptr+pcbGPR6];
  GPR[7]=MainMemory[PCBptr+pcbGPR7];
  // Restore SP and PC from given PCB
  SP=MainMemory[PCBptr+pcbSP];
  PC=MainMemory[PCBptr+pcbPC];
  // Set system mode to User mode
  //PSR = UserMode; // UserMode is 2, OSMode is 1.
  PSR=UserMode;

  return;
}  // end of Dispatcher() function


//Terminate Process

void TerminateProcess (long pcb_pointer)//Method by Connor Huggan
{
  // Return stack memory using stack start address and stack size in the given PCB

  FreeUserMemory(MainMemory[pcb_pointer + pcbStackStartAddr], MainMemory[pcb_pointer + pcbStackSize]);
  FreeOSMemory(pcb_pointer, PCBsize);
  // Return PCB memory using the PCBptr

  return;
}  // end of TerminateProcess function()

//Allocate OS Memory

long AllocateOSMemory (long RequestedSize)//Method by Fraz Ikram
 // return value contains address or error
{

  // Allocate memory from OS free space, which is organized as link
     if(OSFreeList == EndOfList)
     {
       printf("%s\n","No Free Memory" );
      return(ErrorNoFreeMemory);   // ErrorNoFreeMemory is constant set to < 0
      }
     if(RequestedSize < 0)
     {
      printf("Error, Invalid MEMSIZE");
      return(ErrorInvalidMemorySize);  // ErrorInvalidMemorySize is constant < 0
     }
      if(RequestedSize == 1)
      RequestedSize = 2;  // Minimum allocated memory is 2 locations

       long CurrentPtr = OSFreeList;
       long PreviousPtr = EndOfList;
    while (CurrentPtr != EndOfList)
    {
       // Check each block in the link list until block with requested memory size is found
       if(MainMemory[CurrentPtr + 1] == RequestedSize)
      {  
        // Found block with requested size.  Adjust pointers
        if(CurrentPtr == OSFreeList)  // first block
        {
           OSFreeList = MainMemory[CurrentPtr];  // first entry is pointer to next block
           MainMemory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
           return(CurrentPtr); // return memory address
        }
        else  // not first black
        {
          MainMemory[PreviousPtr] = MainMemory[CurrentPtr];  // point to next block
          MainMemory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
          return(CurrentPtr);    // return memory address
        }
      }
      else if(MainMemory[CurrentPtr + 1] > RequestedSize)
    {  
  // Found block with size greater than requested size
        if(CurrentPtr == OSFreeList)  // first block
        {
        MainMemory[CurrentPtr + RequestedSize] = MainMemory[CurrentPtr];  // move next block ptr
        MainMemory[CurrentPtr + RequestedSize + 1] = MainMemory[CurrentPtr +1] - RequestedSize;
        OSFreeList = CurrentPtr + RequestedSize;  // address of reduced block
        MainMemory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
          return(CurrentPtr); // return memory address
        }
        else  // not first black
        {
        MainMemory[CurrentPtr + RequestedSize] = MainMemory[CurrentPtr];  // move next block ptr
        MainMemory[CurrentPtr + RequestedSize + 1] = MainMemory[CurrentPtr +1] - RequestedSize;
        MainMemory[PreviousPtr] = CurrentPtr + RequestedSize;  // address of reduced block
        MainMemory[CurrentPtr] = EndOfList;  // reset next pointer in the allocated block
        return(CurrentPtr); // return memory address
        }
  }
     else // small block 
    {  
    // look at next block
         PreviousPtr = CurrentPtr;
         CurrentPtr = MainMemory[CurrentPtr];
    }
} // end of while CurrentPtr loop

      printf("%s\n","Error No Free OS Memory" );
      return(ErrorNoFreeMemory);   
      // ErrorNoFreeMemory is constant set to < 0
      
}  // end of AllocateOSMemory() function

//Free OS Memory
long FreeOSMemory (long ptr, long size) //Method by Connor Huggan
 // return value contains OK or error code
{
   if(size == 1)
  {
    size = 2;
  }
  if(ptr < 7500 || ptr >9999)
  {
    printf("ERROR: InvalidMemoryAddress \n");
    return(ErrorInvalidAddress);
  }
  else if(size < 1 || (size + ptr) > 9999)
  {
    printf("ERROR: InvalidMemoryAddress \n");
    return(ErrorInvalidSizeORMemoryAddress);
  }
  OSFreeList = 7500;
  MainMemory[OSFreeList] = EndOfList;
  MainMemory[OSFreeList + 1] = 2500;

  return(Okay);

}  // end of FreeOSMemory() function

//Allocate User Memory

long AllocateUserMemory (long size)
  // return value contains address or error code
{
  // Allocate memory from User free space, which is organized as link
  // copy OS code and modify to use UserFreeList
  if(UserFreeList == EndOfList)
  {
    printf("ERROR: No Free Memory\n");
    return(ErrorNoFreeMemory);
  }
  if(size < 0)
  {
    printf("ERROR: Invalid Memory Size\n");
    return(ErrorInvalidMemorySize);
  }
  if(size == 1)
  {
    size = 2;
  }
  long currentPtr = UserFreeList;
  long previousPtr = EndOfList;

  while(currentPtr != EndOfList)
  {
    if(MainMemory[currentPtr +1] == size)
    {
      if(currentPtr == UserFreeList)
      {
        UserFreeList = MainMemory[currentPtr];
        MainMemory[currentPtr] = EndOfList;
        return(currentPtr);
      }
      else
      {
        MainMemory[previousPtr] = MainMemory[currentPtr];
        MainMemory[currentPtr] = EndOfList;
        return(currentPtr);
      }
    }   
    else if(MainMemory[currentPtr + 1] > size)
    {
      if(currentPtr == UserFreeList)
      {
        MainMemory[currentPtr + size] = MainMemory[currentPtr];
        MainMemory[currentPtr + size + 1] = MainMemory[currentPtr + 1];
        UserFreeList = currentPtr + size;
        MainMemory[currentPtr] = EndOfList;
        return(currentPtr);
      }
      else
      {
        MainMemory[currentPtr + size] = MainMemory[currentPtr];
        MainMemory[currentPtr + size + 1] = MainMemory[currentPtr + 1];
        MainMemory[previousPtr] = currentPtr + size;
        MainMemory[currentPtr] = EndOfList;
        return(currentPtr);
      }
    }
    else
    {
      previousPtr = currentPtr;
      currentPtr = MainMemory[currentPtr];
    }
  }
}  // end of AllocateUserMemory() function

//Free User Memory

long FreeUserMemory (long ptr,  long size)  //Method by Fraz Ikram
// return value contains OK or error code
{
  
  // Return memory to User free space.  
// Insert the returned free block at the beginning of the link list

  // This code is similar to free OS memory

     if (ptr > 7499|| ptr <2500 )  // user memory area is given in the class
     {
  		printf("%s\n","ErrorInvalidAddress");
  		return(ErrorInvalidAddress);  
  		// ErrorInvalidMemoryAddress is constantset to  < 0
     }
  // Check for invalid size and minimum size
     if(size ==1)  // check for minimum allocated size, which is 2 even if user asks for 1 location
     {
  		size = 2;  // minimum allocated size
      }
     else if((size < 1) || ((ptr+size) > 7499))  // invalid size
     {  // Invalid size
  		printf("%s\n","Error: Invalid address or size");
 		return(ErrorInvalidSizeORMemoryAddress);    // All error codes are < 0
     } 
  // Insert the free block at the beginning of the link list
     UserFreeList=2500;
     MainMemory[UserFreeList]= EndOfList;
     MainMemory[UserFreeList+1]=5000; 

     return (Okay);
}  // end of FreeUserMemory() function

//Check and Process Interrupt

void CheckAndProcessInterrupt()//Method by Connor Huggam
{
  
  printf("0 - no interrupt\n");
  printf("1 - run program\n");
  printf("2 - shutdown system\n");
  printf("3 - input operation completion\n");
  printf("4 - output operation completion\n");

  int intID;
  printf("Please enter which interrupt ID =>\n");
  scanf("%d", &intID);

  switch(intID)
  {
    case 0: break;
    case 1: ISRrunProgramInterrupt();
    break;
    case 2: ISRshutdownSystem();
    break;
    case 3: ISRinputCompletionInterrupt();
    break;
    case 4: ISRoutputCompletionInterrupt();
    break;
    default: printf("ERROR invalid interrupt ID\n");
    break;

  }
  return;
  return;
}  // end of CheckAndProcessInterrupt() function

//Run Program Interrupt Service Routing
void ISRrunProgramInterrupt()//Fraz Ikram
{
  /*
  Prompt and read filename;
  Call Create Process passing filename and Default Priority as arguments;
  */
  char file[100];
  printf("%s\n","File Name: ");
  scanf("%s", &file);
  CreateProcess(file,DefaultPriority);
  return;
}  // end of ISRrunProgram() function

//Input Completion interrupt
void ISRinputCompletionInterrupt()//Method by Fraz Ikram
{
  //Prompt and read PID of the process completing input completion interrupt;
  printf("%s\n","PID:");
  long pid= scanf("%ld");
  //Search WQ to find the PCB having the given PID
  long PCBptr= SearchAndRemovePCBfromWQ(pid);
    if(PCBptr==EndOfList)
    {
      //Remove PCB from the WQ;
      PCBptr= RQ;
      while(PCBptr != EndOfList)
      {
        //Read one character from standard input device keyboard;
        //Set process state to Ready in the PCB;
        //Insert PCB into RQ;
        if(MainMemory[PCBptr+pcbPID]==pid)
        {
          //Reading character from standard input
          printf("%s\n","Please enter char" );
           char c=scanf("%s");
         //Store the character in the GPR in the PCB;   
         // type cast char to long
          long ch= (long)c;
          MainMemory[PCBptr+pcbGPR0]=ch;

        }
        else
        {
          PCBptr=MainMemory[PCBptr+MainMemory[NextPointer]];
        }
      }
    }

  //If no match is found in WQ, then search RQ
  else
  {
     printf("%s\n","Please enter char" );
      char c=scanf("%s");
    //Read one character from standard input device keyboard;
    //Store the character in the GPR in the PCB;
      long ch= (long)c;
      MainMemory[PCBptr+pcbGPR0]=ch;
      InsertIntoRQ(PCBptr);
  }
  

  //If no matching PCB is found in WQ and RQ, print invalid pid as error message;
    
  return;
}  // end of ISRinputCompletionInterrupt() function

//Output Completion Interrupt
void ISRoutputCompletionInterrupt()//method by connor Huggan
{
  int foundRQ;
  int PID;
  printf("Enter PID =>\n");
  scanf("%d", &PID);

  long pcb_pointer = SearchAndRemovePCBfromWQ(PID);
  if(pcb_pointer == EndOfList)
  {
    pcb_pointer = RQ;
    while(pcb_pointer != EndOfList)
    {
      if(MainMemory[pcb_pointer + pcbPID] == PID)
      {
        char i = MainMemory[pcb_pointer + pcbGPR7] + '0';
        printf("The character is: %c\n", i);
      }
      else
      {
        pcb_pointer = MainMemory[pcb_pointer+ NextPointer];
      }
    }
    if(!foundRQ)
    {
      printf("No matching PID found in RQ\n");
    }
  }
  else
  {
    char c = MainMemory[pcb_pointer + pcbGPR7] + '0';
    printf("The character is: %c\n", c);
    MainMemory[pcb_pointer + PCBState] = ReadyState;
    InsertIntoRQ(pcb_pointer);
  }
  return;

}  // end of ISRonputCompletionInterrupt() function

//Shutdown Sytem Interrupt
void ISRShutdownSystem()
{
  shutdown = 1;
  long pointer;

  pointer = RQ;

  while(pointer != EndOfList)
  {
    RQ = MainMemory[RQ + NextPointer];
    TerminateProcess(pointer);
    pointer = RQ;
  }
  pointer = WQ;
  while(pointer != EndOfList)
  {
    WQ = MainMemory[WQ + NextPointer];
    TerminateProcess(pointer);
    pointer = WQ;
  }
  return;
} // end of ISRshutdownSystem() function

//Search and Remove PCB from WQ
long SearchAndRemovePCBfromWQ ( long pid) //Method by Fraz Ikram
{
  
  long currentPCBptr = WQ;
  long previousPCBptr = EndOfList;

  // Search WQ for a PCB that has the given pid
  // If a match is found, remove it from WQ and return the PCB pointer
  while (currentPCBptr != EndOfList)
  {
    if(MainMemory[currentPCBptr + pcbPID] == pid)
    {
      // match found, remove from WQ
      if(previousPCBptr == EndOfList)
      { // first PCB
        WQ = MainMemory[currentPCBptr + NextPointer];
      }
      else
      { // not first PCB
        MainMemory[previousPCBptr+NextPointer] = MainMemory[currentPCBptr+NextPointer];
      }
      MainMemory[currentPCBptr+NextPointer] = EndOfList;
      return(currentPCBptr);
    }
    previousPCBptr = currentPCBptr;
    currentPCBptr = MainMemory[currentPCBptr + NextPointer];
  }  // end while currentPCBptr

  // No matching PCB is found, display pid message and return End of List code
  printf("%s\n", "No matching PID found in Waiting Queue");

  return (EndOfList);
}  // SearchAndRemovePCBfromWQ



































 






