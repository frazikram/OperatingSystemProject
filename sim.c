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

/* return(ExecutionCompletionStatus);  	// return from main */

  char filename[30]; /*   // Declare local variable here */
  long ExecutionCompletionStatus;
  InitializeSystem();
  // Read hypo machine language executable filename to run from the user */
  printf("Enter filename"); /*  display prompt asking user to enter the filename; */
  scanf("%s",filename);
   // Load the program */
  printf("Entered name: %s\n", filename );
  long returnValue=AbsoluteLoader(filename);
  if(returnValue<-1) /*   Check for error and return error code; */
  {
    printf("Error: %ld\n",returnValue );
  }
  else
  {
    PC= returnValue; /*   Set program counter (pc) to returnValue; */
  }
  printf("PC: %ld\n",PC);
  DumpMemory("After loading user program", 0, 99);/*  Dump memory after loading user program: Range 0 to 99 locations */
  ExecutionCompletionStatus=CPU();/*  // Execute HYPO Machine program by calling CPU method (function) */
  DumpMemory("After executing user program",0,99);/*  Dump memory after executing user program: Range 0 to 99 locations */
  printf("Execution Complete Status: %ld\n",ExecutionCompletionStatus );

  return ExecutionCompletionStatus; 

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
}  // end of DumpMemory() 




