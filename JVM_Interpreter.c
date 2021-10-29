#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h> 
#include <stdbool.h>
#include "VirtualMachineConfig.h"
#include<arpa/inet.h>

//____________________________________________________________________________________________________________________________________________
//THIS WAS SUBMITED AS ASSIGNEMENT : 1 & 2
//____________________________________________________________________________________________________________________________________________
//.Desciption:   The interpreter will interprete JAVA Byte code
//.Author:       Subhabrata Rana
//.Compile:      gcc JVM_Interpreter.c -o JVM_Interpreter
//.Run Program: ./JVM_Interpreter -e exec -c constant_pool arg
//.Example:     ./JVM_Interpreter -e Fibonacci.bvmclass -c Fibonacci.constantpool 4
//________________________________________________________________________________________________________________________________________________________________________
//EXPECTED PARAMETERS :  argv[0]:./JVM_Interpreter // argv[1]:-e  // argv[2]:fileName.bvmclass   // argv[3]:-c    // argv[4]: fileName.constantPool   // argv[5]:arg
//________________________________________________________________________________________________________________________________________________________________________

    #define SEGMENTATION_PROGRAM 1024
    #define SEGMANETAION_CONSTANT_POOL 256
    #define SEGMENTATION_OBJECT_REF_CONST_POOL 768
    #define SEGMENTATION_STACK_FRAME 1048576
    #define MAX_VM_SIZE 4294967295 // Pow(2,32)-1


    // Declaration of Pointers
     int *JVM=NULL;                                                // This pointer will point to the starting address of the entire VM 
     int *JVM_End=NULL;                                            // This pointer will point to the end of the VM
     int *heapPointer_VM;                                          // This will initially point to the JVM_End and eventually will grow downwards
     unsigned char *program_Pointer_VM=NULL;                         // This pointer should starts from memery 1024 in the VM     
     //unsigned char *program_Pointer_VM=NULL; 
     
     unsigned char *constantPool_Pointer_VM=NULL;                    // This pointer should start from memory 256 in the VM
     unsigned char *objectReference_In_ConstantPool=NULL;            // This pointer will point to the Object Reference in Constant Pool at 768 
     unsigned char *stackFrame_Pointer_VM=NULL;                      // This pointer will point to  Stack Frame in VM     
     unsigned char *interpreter_ProgramCounter=NULL;               // This poinrer will same as program_Pointer_VM but it is unsigned char pointer
     int *operandStack_Pointer=NULL;                               // This pointer will point to the Operand Stack
     int *stackFrame_localVariableIndexCounter=NULL;               // This pointer will point to the Local and parameters inside the Stack Frame
     int *stackFramePointer_Dynamic=NULL;                          // This pointer will be dynamic and will increase to the end of the Stack Frame
     int *stackFramePointer_Static=NULL;                           // This pointer will be static and will always point to the start of the stack frame
     int valueOfParameter;         
     int *argumentPointer_InvokeStatic=NULL;                       // This pointer will hold the value passing from the main method as arg---TODO 
     int *stackFrame_Pointer_In_MainMethod=NULL;                   // This pointer will decide whether the function finished executing the all operation and return     

    char *header; 
    int execFileLength;
    int constantPoolFileLength;
//________________________________________________________________________________________________________________________________________________________________________
// MAIN INTERPRETER START
 //________________________________________________________________________________________________________________________________________________________________________
  
int main(int argc, char* argv[])
{

      reset();
      // Below Function will Take the Number of Argument passing from Command Line and Save int Array. 
      // This will be beneficial when we pass more than 1 argument from the command line as arg
      int numberOfArgs=argv_length(argv);
      int numberOfParameters=numberOfArgs-5;
      int *argPointer;
      int arrayParameters[16]={0};         // Initialize the variables as zero to discard garbage values
      argPointer=arrayParameters;        
      int arrayIndex=0;
      int result;
      int arugmentIndex=0;

      // Get the Parameters passing from Command Line as Argument to Main method
      for(arrayIndex=0;arrayIndex<numberOfParameters;arrayIndex++)
      { 
        arugmentIndex=5+arrayIndex;
        result=atoi(argv[arugmentIndex]);
        arrayParameters[arrayIndex]=result;         
      }
  

     red();
     printf("\nJVM Interpreter program started\n"); 
     reset();


     // Allocate memory for the Virtual Machine. It will return the pointer to the starting of the Entire Virtual Machine   
     JVM=(int *)malloc(MAX_VM_SIZE); 

     // Validate the Memory Allocation
     if(JVM==NULL)
     {
       red();
       printf("\nMemory allocation failed for the entire VM at location : %ld (%p)",JVM,JVM);
       exit(EXIT_FAILURE);
       reset();
     }

    yellow();
    printf("\nMemory allocation successful at location : %ld (%p)",JVM,JVM);           
     reset();   


     //___________________________________________________________________________________________________________________________
     //Assignment of the Pointers
     //___________________________________________________________________________________________________________________________
     
     // Pointer-->JVM_End
     JVM_End=(int *)((char *)JVM+MAX_VM_SIZE);                              // Needed to convert to Char Pointer first then again int to properly start from MAX_VM_SIZE

     // Pointer-->Heap Pointer at VM                                        // Initially will points to the JVM_End
         heapPointer_VM=JVM_End;                                       
    
     // Pointer-->Program Counter                                            // Pointer to the 1024 Location
      program_Pointer_VM= (unsigned char *)JVM+SEGMENTATION_PROGRAM;
      //printf("\n Program File\n");

      header="Program File";
      Printheader(header);
      execFileLength=LoadBinaryFileIntoVM(argv,2,SEGMENTATION_PROGRAM,JVM);  

      //Pointer-->Constant Pool                                             // Pointer to the 256 location
      constantPool_Pointer_VM= (unsigned char *)JVM+SEGMANETAION_CONSTANT_POOL;

      header="Constant Pool File";
      Printheader(header);
    
      //printf("\n Constant Pool File\n");
      constantPoolFileLength=LoadBinaryFileIntoVM(argv,4,SEGMANETAION_CONSTANT_POOL,JVM);   
      
      // Pointer-->Object Reference in the Constant Pool                     // Pointer to the 768 location
      objectReference_In_ConstantPool=(unsigned char *)JVM+SEGMENTATION_OBJECT_REF_CONST_POOL;    
      
      // Pointer-->Stack Frame                                                // Pointer to the Stack Frame
      stackFrame_Pointer_VM=(unsigned char *)JVM+SEGMENTATION_STACK_FRAME;  
      
      // Pointer--> stackFramePointer_Static.  stackFramePointer_Dynamic       // Converting the StakeFrame_Pointer-VM from signed Char to Int     
      stackFramePointer_Static=(int *) (stackFrame_Pointer_VM);                // It will always points to the starting point of the Stack Frame
      stackFramePointer_Dynamic=(int *) (stackFramePointer_Static+19);         // This is will be dynamic and will increase to the end of the Stack Frame
     
      // Pointer-->  stackFrame_Pointer_In_MainMethod
      stackFrame_Pointer_In_MainMethod=stackFramePointer_Static=(int *) (stackFrame_Pointer_VM);  // This will keep track of the Stack Frame and will help in deciding whether we are in Main Method
      
      // Pointer-->Program Counter                                            // Converting the PC to unsigned Char. 
      //interpreter_ProgramCounter=(unsigned char*)program_Pointer_VM;
     interpreter_ProgramCounter=program_Pointer_VM; //SUBHA
      // Pointer-->Local Variables and Parameters inside Stack Frame
      stackFrame_localVariableIndexCounter=stackFramePointer_Static+4;  
 
     // Pointer-> Operand Stack                                                // This will point to the Stack Top(TOP=-1) location. When we need to PUSH, we need to first increment the Stack Top
      operandStack_Pointer=stackFramePointer_Static+19;

     // Print entire Virtual Machine
        
     header="Entire Virtual Machine";
    Printheader(header);
     PrintVirtualMachine(JVM,program_Pointer_VM,execFileLength,constantPool_Pointer_VM,constantPoolFileLength,stackFramePointer_Static);

     //___________________________________________________________________________________________________________________________
     //Assign Values for Creating Initial Stack Frame for Main Method
     //___________________________________________________________________________________________________________________________
     uint32_t programCounter_Address;    
     programCounter_Address=0;                       // INITIALIZATION-MAIN METHOD: Previous program counter is zero
     
     uint32_t stackFrame_ReturnValue;
     stackFrame_ReturnValue=0;                       // INITIALIZATION-MAIN METHOD: Return Value is zero

     uint32_t stackFrame_Address;   
    stackFrame_Address=0;                            // INITIALIZATION-MAIN METHOD: Previous Stack Frame Address is zero

     uint32_t numberOfArguments;
     numberOfArguments=numberOfParameters;           // INITIALIZATION-MAIN METHOD: Number of Argument is same as passed to Main Method

     uint32_t ArgumentType;
     ArgumentType=0;                                 // INITIALIZATION-MAIN METHOD: Number of Argument Type is zero for Integer        

     uint32_t operandStack_Address;                 
     operandStack_Address=0;                         // INITIALIZATION-MAIN METHOD: Previous Operand Stack address is zero    

    // Initialize Stack Frame with the Values
    SaveValueInStackFrame(stackFrame_ReturnValue,programCounter_Address,operandStack_Address,stackFrame_Address,numberOfArguments,argPointer);

     // Print Initialized Stack Frame
      
    header="INSIDE MAIN METHOD: Stack Frame. Operand Stack and heap";
     Printheader(header);
     PrintStackFrame();
    
    // Get the First Opcode from the Execution File
      uint32_t opcode=(uint32_t)(*interpreter_ProgramCounter);   
      char *opcodeName; 
      int numberOfByteCodes=0;  
  
      
      header="Program Execution Started";
      Printheader(header);
    
     
     for(numberOfByteCodes=0;numberOfByteCodes<execFileLength-1;numberOfByteCodes++)
     {   
                            
       // Halt while hitting return
         while (opcode != 0xb1) {         
         uint32_t opcode=(uint32_t)(*interpreter_ProgramCounter);     
      
        switch (opcode) {

                 
                case 0X10: 
                            opcodeName="BIPUSH";              
                            PrintOpcode(opcode,opcodeName);                                                          
                     BIPUSH();                    
	                   break;  
                 case 0X36:  
                            opcodeName="ISTORE";              
                            PrintOpcode(opcode,opcodeName);                          
                                                    
                      ISTORE(); 
                      break; 
                 case 0X03:  
                                   opcodeName="ICONST_0";              
                                   PrintOpcode(opcode,opcodeName); 
                                                                       
                      ICONST_0();
                      break; 
                case 0X15: 
                                   opcodeName="ILOAD";              
                                   PrintOpcode(opcode,opcodeName);                                                              
                      ILOAD();                      
                      break;
                 case 0X9d:  
                                   opcodeName="IFGT";              
                                   PrintOpcode(opcode,opcodeName);                                                                
                       IFGT();              
                       break; 
                 case 0Xa7:  
                                   opcodeName="GOTO";              
                                   PrintOpcode(opcode,opcodeName); 
                                 
                      GOTO();  
                      break; 
                 case 0X64:   
                                     opcodeName="ISUB";              
                                     PrintOpcode(opcode,opcodeName);         
  
                      ISUB();             
                      break;
                 case 0X60: 
                                     opcodeName="IADD";              
                                     PrintOpcode(opcode,opcodeName);  
  
                      IADD(); 
                      break;
                case  0X9f:   
                                   opcodeName="ICMPEQ";              
                                    PrintOpcode(opcode,opcodeName);    
                      
                         ICMPEQ();                              
                         break; 
                case 0x84: 
                                    opcodeName="IINC";              
                                    PrintOpcode(opcode,opcodeName); 
                      IINC();
                      break;
                case 0xff:  
                                   opcodeName="IWRITE";              
                                   PrintOpcode(opcode,opcodeName); 

                      IWRITE();                                   
                      break;              
                case 0X2e: 
                                   opcodeName="IALOAD";              
                                   PrintOpcode(opcode,opcodeName); 
                      IALOAD();            
                      break; 
                case 0X7e: 
                                  opcodeName="IAND";              
                                   PrintOpcode(opcode,opcodeName);  

                      IAND();
                      break; 
                case 0X4f: 
                                  opcodeName="IASTORE";              
                                   PrintOpcode(opcode,opcodeName);  

                       IASTORE();      
                       break; 
                case 0X6c: 
                                   opcodeName="IDIV";              
                                   PrintOpcode(opcode,opcodeName);  
                      IDIV();                  
                      break; 
                case 0X99:    
                                  opcodeName="IFEQ";              
                                   PrintOpcode(opcode,opcodeName);   
                      IFEQ(); 
                      break;
              case 0X9b:    
                                   opcodeName="IFLT";              
                                   PrintOpcode(opcode,opcodeName);  
                     IFLT(); 
                     break; 
                case 0X9c:    
                                   opcodeName="IFGE";              
                                   PrintOpcode(opcode,opcodeName);            
                      IFGE();
                      break;    
              case 0X9e:    

                                  opcodeName="IFLE";              
                                   PrintOpcode(opcode,opcodeName);   
                     IFLE();           
                     break;  
               case 0Xc6:   
                                   opcodeName="IFNULL";              
                                   PrintOpcode(opcode,opcodeName);   
                     IFNULL();         
                     break;  
              case 0X68:    
                                  opcodeName="IMULL";              
                                   PrintOpcode(opcode,opcodeName); 
                    IMULL();              
                    break; 
              case 0X74:   
                                   opcodeName="INEG";              
                                   PrintOpcode(opcode,opcodeName); 
                     INEG();    
                    break;                
                case 0X80:   
                                  opcodeName="IOR";              
                                   PrintOpcode(opcode,opcodeName); 
                      IOR();               
                      break; 
              case 0X57:    
                                   opcodeName="POP";              
                                   PrintOpcode(opcode,opcodeName);          
                    POP_OperandStack();          
                     break; 
              case 0Xac:    
                                   opcodeName="IRETURN";              
                                   PrintOpcode(opcode,opcodeName);   
                      IRETURN();
                      break;             
               case 0xb1: 
                                   opcodeName="RETURN";              
                                   PrintOpcode(opcode,opcodeName);         

                    RETURN();                                  
                    break;

          case 0Xb8:   
                                  opcodeName="INVOKE_STATIC";              
                                   PrintOpcode(opcode,opcodeName);         

                    INVOKE_STATIC();
                    break; 
          case 0xfe:  
                                  opcodeName="IREAD";              
                                   PrintOpcode(opcode,opcodeName);    
                 IREAD();
                 break;
          case 0xbb:     
                                   opcodeName="NEW";              
                                   PrintOpcode(opcode,opcodeName);                                                   
                    
                 NEW();                 
                 break;
          case 0xbd: 
                                    opcodeName="ANEWARRAY";              
                                   PrintOpcode(opcode,opcodeName);
                  ANEWARRAY();
                 break;

            case 0xbc:                                                     
                                  opcodeName="NEWARRAY";              
                                   PrintOpcode(opcode,opcodeName);
                 NEWARRAY();
                 break;
             case 0xb4:      
                                   opcodeName="GETFIELD";              
                                   PrintOpcode(opcode,opcodeName);
                 
                  GETFIELD(); 
                  break;
            case 0xb5:            
                                   opcodeName="PUTFIELD";              
                                   PrintOpcode(opcode,opcodeName);
            
                    PUTFIELD();             
                     break;
              case  0x19:    
              
                                   opcodeName="ALOAD";              
                                   PrintOpcode(opcode,opcodeName);
                ALOAD();    
                break;
            case  0x3a:           
                                  opcodeName="ASTORE";              
                                   PrintOpcode(opcode,opcodeName);
                   ASTORE();   
                   break;
            case  0x59:
                                  opcodeName="DUP";              
                                   PrintOpcode(opcode,opcodeName);
                DUP();   
                break;

            case    0x1: 
                                   opcodeName="ACONST_NULL";              
                                   PrintOpcode(opcode,opcodeName);
             ACONST_NULL();
             break;

          case    0x53: 
                                   opcodeName="AASTORE";              
                                   PrintOpcode(opcode,opcodeName);
             AASTORE();
             break;
           case    0x32: 
                                   opcodeName="AALOAD";              
                                   PrintOpcode(opcode,opcodeName);
             AALOAD();
             break;
           default:// The exit (0) shows the successful termination of the program and the exit(1) shows the abnormal termination of the program
           
            //PrintStackFrame();
            exit(0);
  
            }
         }   

     }
}



//____________________________________________________________________________________________________________________________
// This will print the Virual Machine
//____________________________________________________________________________________________________________________________
void PrintVirtualMachine(int * JVM,unsigned char * program_Pointer_VM,int execFileLength,unsigned char * constantPool_Pointer_VM,int constantPoolFileLength,int * stackFramePointer_Static)
{

    unsigned char *OS_Karnel_Bottom=(unsigned char *)JVM;
    unsigned char *OS_Kernel_Top=(unsigned char *)JVM+SEGMANETAION_CONSTANT_POOL-1;
    unsigned char *Const_Pool_Top=(unsigned char *)JVM+SEGMENTATION_PROGRAM-1;
    unsigned char *Const_Pool_FileEnd=(unsigned char *)JVM+SEGMANETAION_CONSTANT_POOL+constantPoolFileLength;
    unsigned char *Const_Pool_bottom=(unsigned char *)JVM+SEGMANETAION_CONSTANT_POOL;
    unsigned char *Exec_Top=(unsigned char *)JVM+SEGMENTATION_STACK_FRAME-1;
    unsigned char *Exec_FileEnd=(unsigned char *)JVM+SEGMENTATION_PROGRAM+execFileLength;
    unsigned char *Exec_Bottom=(unsigned char *)JVM+SEGMENTATION_PROGRAM;
   

    printf("\n  VM TOP,Heap Pointer     ****************************************%ld (%p)",JVM_End,JVM_End);
    printf("\n                          ***************************************");
    printf("\n                          ***************************************");
    printf("\n                          ***************************************"); 
    printf("\n                          ***************************************");
    printf("\n                          ***************************************");
    printf("\n                          ***************************************");   
    printf("\nStackFrame End,OperandStack****************************************%ld (%p)",stackFramePointer_Static+19,stackFramePointer_Static+19);
    printf("\n                          ****************************************");
    printf("\n                          ****************************************");
    printf("\n                          ****************************************");
    printf("\nStack Frame Start         ***************************************%ld (%p)",stackFramePointer_Static,stackFramePointer_Static);
    printf("\n                          ****************************************");
    printf("\n                          ****************************************");
    printf("\nProgrm Counter End        ****************************************%ld (%p)",Exec_Top,Exec_Top);
    printf("\n                          ***************************************");
    printf("\n                          ***************************************");
    printf("\n                          ***************************************");
    printf("\nExec File Copy End        ****************************************%ld (%p)",Exec_FileEnd, Exec_FileEnd);
    printf("\n                          ***************************************");
    printf("\n                          ***************************************");
    printf("\nProgram Counter Start     ****************************************%ld (%p)",Exec_Bottom, Exec_Bottom);
    printf("\nConst Pool Pointer End    ****************************************%ld (%p)",Const_Pool_Top,Const_Pool_Top);
    printf("\n                          **************************************");
    printf("\n                          **************************************");
    printf("\n                          **************************************");
    printf("\n                          **************************************");   
    printf("\nConst Pool File End       ****************************************%ld (%p)",Const_Pool_FileEnd, Const_Pool_FileEnd);
    printf("\n                          **************************************");
    printf("\nObject Reference Start    ****************************************%ld(%p)",objectReference_In_ConstantPool, objectReference_In_ConstantPool);
    printf("\n                          **************************************");
    printf("\n                          **************************************");
    printf("\nConst Pool Pointer Start  ****************************************%ld (%p)",Const_Pool_bottom,Const_Pool_bottom);
    printf("\nOS Kernel End             ****************************************%ld (%p)",OS_Kernel_Top,OS_Kernel_Top);
    printf("\n                          **************************************");
    printf("\nOS Kernel Start           ***************************************%ld (%p)\n",OS_Karnel_Bottom,OS_Karnel_Bottom);


}

//____________________________________________________________________________________________________________________________
// Helper functions to reduce pointer size 
// Pointers at location end of JVM is large. An int size pointer will not be able to hold the entire address
// We need to save the logical reference of the address by converting it into a relative address of the pointer in respect to JVM 
//____________________________________________________________________________________________________________________________

//___________________________________________________________________________________________________________________________
// This Method will chage to Larger Pointer Address **TO LOGICAL ADDRESS by reducing it from JVM Start Address
//_____________________________________________________________________________________________________________________________
uint32_t Convert_To_LogicalAddres(void* physical_Address)
{
  return (uint32_t)((long)physical_Address - (long)(JVM));
}

//____________________________________________________________________________________________________________________________
// This method will change back to the actual physical location  **FROM  LOGICAL ADDRESS
//____________________________________________________________________________________________________________________________
int * Convert_To_PhysicalAddress(uint32_t logical_Address)
{
 return (void*)JVM+logical_Address;
}

//____________________________________________________________________________________________________________________________
// Colors to  Print in the console
//____________________________________________________________________________________________________________________________
void red () { printf("\033[1;31m");}
void yellow (){ printf("\033[1;33m");}
void green () {printf("\033[0;32m");}
void blue () {printf("\033[0;34m");}
void reset () {printf("\033[0m");}


//____________________________________________________________________________________________________________________________
// This Method should create a Stake Frame and Value Stack on the Top of the Stack
//____________________________________________________________________________________________________________________________
void PrintStackFrame(){
    
    
    // header="Heap , Stack Frame and Operand Stack";
     //Printheader(header);
 
    blue();
    printf("\nVM TOP,Initial Heap Pointer  |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End),JVM_End,JVM_End);
    printf("\nVM TOP-1                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-1),JVM_End-1,JVM_End-1);
    printf("\nVM TOP-2                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-2),JVM_End-2,JVM_End-2);
    printf("\nVM TOP-3                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-3),JVM_End-3,JVM_End-3);
    printf("\nVM TOP-4                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-4),JVM_End-4,JVM_End-4);
    printf("\nVM TOP-5                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-5),JVM_End-5,JVM_End-5);
    printf("\nVM TOP-6                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-6),JVM_End-6,JVM_End-6);
    printf("\nVM TOP-7                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-7),JVM_End-7,JVM_End-7);
    printf("\nVM TOP-8                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-8),JVM_End-8,JVM_End-8);
   // printf("\nVM TOP-9                     |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-9),JVM_End-9,JVM_End-9);
    //printf("\nVM TOP-10                    |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-10),JVM_End-10,JVM_End-10);
   // printf("\nVM TOP-11                    |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-11),JVM_End-11,JVM_End-11);
   // printf("\nVM TOP-12                    |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-12),JVM_End-12,JVM_End-12);
   // printf("\nVM TOP-13                    |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-13),JVM_End-13,JVM_End-13);
   // printf("\nVM TOP-14                    |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-14),JVM_End-14,JVM_End-14);
   // printf("\nVM TOP-15                    |\t\t%u\t\t| Address: %ld (%p)\n",*(JVM_End-15),JVM_End-15,JVM_End-15);
    reset();
    printf("\n                             |\t\t\t\t|");
    red();
    printf("\n** Current HEAP TOP          |\t\t%u\t\t| Address: %ld (%p)\n",*(heapPointer_VM),heapPointer_VM,heapPointer_VM);
    reset();
    printf("\n                             |\t\t\t\t|");
    printf("\n                             |\t\t\t\t|");
    red();
    printf("\n** Current Operand Stack Top:|\t\t%u\t\t| Address: %ld (%p)\n",*(operandStack_Pointer),operandStack_Pointer,operandStack_Pointer);
    reset();
    printf("\n                             |\t\t\t\t|");
    printf("\n                             |\t\t\t\t|");   
    green();
   // printf("\nOperand Stack[14]            |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+34),stackFramePointer_Static+34,stackFramePointer_Static+34);
   // printf("\nOperand Stack[13]            |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+33),stackFramePointer_Static+33,stackFramePointer_Static+33);
   // printf("\nOperand Stack[12]            |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+32),stackFramePointer_Static+32,stackFramePointer_Static+32);
   // printf("\nOperand Stack[11]            |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+31),stackFramePointer_Static+31,stackFramePointer_Static+31);
   // printf("\nOperand Stack[10]            |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+30),stackFramePointer_Static+30,stackFramePointer_Static+30);
   // printf("\nOperand Stack[9]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+29),stackFramePointer_Static+29,stackFramePointer_Static+29);
    printf("\nOperand Stack[8]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+28),stackFramePointer_Static+28,stackFramePointer_Static+28);
    printf("\nOperand Stack[7]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+27),stackFramePointer_Static+27,stackFramePointer_Static+27);
    printf("\nOperand Stack[6]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+26),stackFramePointer_Static+26,stackFramePointer_Static+26);
    printf("\nOperand Stack[5]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+25),stackFramePointer_Static+25,stackFramePointer_Static+25);
    printf("\nOperand Stack[4]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+24),stackFramePointer_Static+24,stackFramePointer_Static+24);
    printf("\nOperand Stack[3]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+23),stackFramePointer_Static+23,stackFramePointer_Static+23);
    printf("\nOperand Stack[2]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+22),stackFramePointer_Static+22,stackFramePointer_Static+22);
    printf("\nOperand Stack[1]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+21),stackFramePointer_Static+21,stackFramePointer_Static+21); 
    printf("\nOperand Stack[0]             |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+20),stackFramePointer_Static+20,stackFramePointer_Static+20);  
    printf("\nOperand Stack[Top-1]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+19),stackFramePointer_Static+19,stackFramePointer_Static+19);
    reset();
    printf("\nLocal and Params [15],[Top-1]|\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+15),stackFrame_localVariableIndexCounter+15,stackFrame_localVariableIndexCounter+15);
    printf("\nLocal and Params [14]        |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+14),stackFrame_localVariableIndexCounter+14,stackFrame_localVariableIndexCounter+14); 
    printf("\nLocal and Params [13]        |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+13),stackFrame_localVariableIndexCounter+13,stackFrame_localVariableIndexCounter+13);
    printf("\nLocal and Params [12]        |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+12),stackFrame_localVariableIndexCounter+12,stackFrame_localVariableIndexCounter+12);
    printf("\nLocal and Params [11]        |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+11),stackFrame_localVariableIndexCounter+11,stackFrame_localVariableIndexCounter+11);
    printf("\nLocal and Params [10]        |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+10),stackFrame_localVariableIndexCounter+10,stackFrame_localVariableIndexCounter+10);
    printf("\nLocal and Params [9]         |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+9),stackFrame_localVariableIndexCounter+9,stackFrame_localVariableIndexCounter+9);
    printf("\nLocal and Params [8]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+8),stackFrame_localVariableIndexCounter+8,stackFrame_localVariableIndexCounter+8);
    printf("\nLocal and Params [7]         |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+7),stackFrame_localVariableIndexCounter+7,stackFrame_localVariableIndexCounter+7);
    printf("\nLocal and Params [6]         |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+6),stackFrame_localVariableIndexCounter+6,stackFrame_localVariableIndexCounter+6);
    printf("\nLocal and Params [5]         |\t\t%u\t\t| Address: %ld (%p) \n",*(stackFrame_localVariableIndexCounter+5),stackFrame_localVariableIndexCounter+5,stackFrame_localVariableIndexCounter+5);
    printf("\nLocal and Params [4]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+4),stackFrame_localVariableIndexCounter+4,stackFrame_localVariableIndexCounter+4);
    printf("\nLocal and Params [3]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+3),stackFrame_localVariableIndexCounter+3,stackFrame_localVariableIndexCounter+3);
    printf("\nLocal and Params [2]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+2),stackFrame_localVariableIndexCounter+2,stackFrame_localVariableIndexCounter+2);
    printf("\nLocal and Params [1]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter+1),stackFrame_localVariableIndexCounter+1,stackFrame_localVariableIndexCounter+1);
    printf("\nLocal and Params [0]         |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFrame_localVariableIndexCounter),stackFrame_localVariableIndexCounter,stackFrame_localVariableIndexCounter);
    yellow();
    printf("\nPrevious Frame Address       |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+3),stackFramePointer_Static+3,stackFramePointer_Static+3);
    printf("\nPrevious OperandStackAddress |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+2),stackFramePointer_Static+2,stackFramePointer_Static+2);
    printf("\nPrevious Program Counter     |\t\t%u\t\t| Address: %ld (%p)\n",*(stackFramePointer_Static+1),stackFramePointer_Static+1,stackFramePointer_Static+1);
    printf("\nStack Frame ReturnValue      |\t\t%d\t\t| Address: %ld (%p)\n",*stackFramePointer_Static,stackFramePointer_Static,stackFramePointer_Static);
    reset();
   }

//____________________________________________________________________________________________________________________________
// START: ALL BYTE CODE INSTRUCTION 
//____________________________________________________________________________________________________________________________
/*Operation
       Push byte
Format
       bipush
       byte
Forms
       bipush = 16 (0x10)
*/
void BIPUSH()
{           int32_t value;
            interpreter_ProgramCounter++; 
           // The immediate byte is sign-extended to an int value. That value is pushed onto the operand stack.
            //value=(uint32_t)((signed char)*interpreter_ProgramCounter);   
            value=(int32_t)((signed char)*interpreter_ProgramCounter);          
           
             Push(value);                                
             interpreter_ProgramCounter++; 
           // PrintStackFrame();  // UNCOMMENT FOR DEBUG
}


/*Operation
     Store int into local variable
 Format
       istore
       index
Forms
      istore = 54 (0x36)
Description
The index is an unsigned byte that must be an index into the local variable array of the current frame (§?2.6).
 The value on the top of the operand stack must be of type int. 
It is popped from the operand stack, and the value of the local variable at index is set to value.
*/
void ISTORE()
{
  interpreter_ProgramCounter++; 
  uint8_t index =(uint8_t)(*interpreter_ProgramCounter);  

  int iResult=(int)(Pop()); 
  (*(stackFrame_localVariableIndexCounter+index))=iResult;  

  //printf("\n\t\t\t Storing Int Value {%u} from index [%d] . Address %ld", iResult,index,(stackFrame_localVariableIndexCounter+index));
  // INCREMENT PC        
  interpreter_ProgramCounter++; 
 
   // UNCOMMENT FOR DEBUG

}


/*
Operation
       Push int constant
Format
     iconst_<i>
Forms  
     iconst_0 = 3 (0x3)
*/
void ICONST_0()
{
   Push(0); 
  interpreter_ProgramCounter++;   // INCREMENT PC
    // UNCOMMENT FOR DEBUG
}



/*
Operation
       Branch if int comparison with zero succeeds
Format
       if<cond>
       branchbyte1
       branchbyte2
Forms
    ifeq = 153 (0x99)
    ifne = 154 (0x9a)
    iflt = 155 (0x9b)
    ifge = 156 (0x9c)
    ifgt = 157 (0x9d)
*/
void IFGT()
{
      // interpreter_ProgramCounter++;                  
      // uint8_t unsigned_a=(uint8_t)(*interpreter_ProgramCounter);  
      // interpreter_ProgramCounter++;              
      // uint8_t unsigned_b=(uint8_t)(*interpreter_ProgramCounter); 



              interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
            
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     

      int iResult=(int)(Pop());              
      if(iResult>0)
      {

          signed short signedShort_offsetAddress=(signed short)((unsigned_a << 8) | unsigned_b);
          interpreter_ProgramCounter--;
          interpreter_ProgramCounter--;                 
          interpreter_ProgramCounter=interpreter_ProgramCounter+signedShort_offsetAddress;              
            
      }
      else
      {
        interpreter_ProgramCounter++; 
      }   

        // UNCOMMENT FOR DEBUG

}

/*goto
   Operation
        Branch always

Format
      goto
      branchbyte1
      branchbyte2
Forms
      goto = 167 (0xa7)
*/
 void GOTO()
 {
              interpreter_ProgramCounter++;                  
              uint8_t  unsigned_a=(uint8_t)(*interpreter_ProgramCounter);

              interpreter_ProgramCounter++;
              uint8_t unsigned_b=(uint8_t)(*interpreter_ProgramCounter);

               int16_t int16_offset=(int16_t)((unsigned_a << 8) | unsigned_b);
               interpreter_ProgramCounter--;
               interpreter_ProgramCounter--;
              
              interpreter_ProgramCounter=interpreter_ProgramCounter+int16_offset; 

               // UNCOMMENT FOR DEBUG

 }
/*
 Operation
     Subtract int
Format
     isub
Forms
     isub = 100 (0x64)
 */
 void ISUB()
 {
                int iValue2 =(int)(Pop()); 
                int iValue1= (int)(Pop());           
                int iResult = iValue1-iValue2;                       
                Push(iResult);                 
                // Increment PC
                 interpreter_ProgramCounter++;
                  // UNCOMMENT FOR DEBUG
 }
 void IADD()
  {
  

                 int iValue1 =(int) (Pop());                 
                 int iValue2= (int) (Pop());              
                int iResult = iValue1+iValue2;                    
                Push(iResult);
                 // Increment PC
                 interpreter_ProgramCounter++;  

                // UNCOMMENT FOR DEBUG

 }
//Both value1 and value2 must be of type int. They are both popped from the operand stack and compared. All comparisons are signed. The results of the comparison are as follows:
 void ICMPEQ()
 {
              // interpreter_ProgramCounter++;       
              // uint8_t unsigned_a=(uint8_t)(*interpreter_ProgramCounter);  

              // interpreter_ProgramCounter++;
              // uint8_t unsigned_b=(uint8_t)(*interpreter_ProgramCounter);     


               interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
              
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     
          

              int iValue1=(int)(Pop()); 
              int iValue2=(int)(Pop()); 
              if(iValue1==iValue2)
              {
                 int16_t int16_offset=(int16_t)((unsigned_a << 8) | unsigned_b);
                 interpreter_ProgramCounter--;
                 interpreter_ProgramCounter--;                 
                 interpreter_ProgramCounter=interpreter_ProgramCounter+int16_offset;                 
              }
              else
              {
                   interpreter_ProgramCounter++; 
              }  

             // UNCOMMENT FOR DEBUG

 }
/*
 Operation
   Increment local variable by constantF
Format
   iinc
   index
   const
Forms
    iinc = 132 (0x84)
   */
 void IINC() //The index is an unsigned byte that must be an index into the local variable array of the current frame (§?2.6).  //The const is an immediate signed byte. The local variable at index must contain an int. The value const is first sign-extended to an int, and then the local variable at index is incremented by that amount.                
 {
               interpreter_ProgramCounter++;               
               uint8_t index_To_LocalVariableArray =(uint8_t)(*interpreter_ProgramCounter);     //The index is an unsigned byte that must be an index into the local variable array of the current frame (§?2.6). 

                interpreter_ProgramCounter++; 
               int8_t  const_value=(int8_t)(*interpreter_ProgramCounter);                    //The const is an immediate signed byte.


               //The local variable at index must contain an int.
               int value_Referenced_By_Index=(int)(*(stackFrame_localVariableIndexCounter+index_To_LocalVariableArray));  

             //The value const is first sign-extended to an int
            // int signExtended_const_value=(int)const_value;
             value_Referenced_By_Index=value_Referenced_By_Index+(int)const_value;     
            *(stackFrame_localVariableIndexCounter+index_To_LocalVariableArray)=value_Referenced_By_Index; 
                            

               // Increment PC
                interpreter_ProgramCounter++;
                // UNCOMMENT FOR DEBUG

 }
 void IWRITE()
 {

                int iResult=(int)(Pop());                 
             
                 yellow();
                 printf("\n\n_______________________________________________________________________________________________________________\n");
                 green();
                 printf("\n\nThe Result is : ");
                 red();
                 printf("   %d",iResult);
                 reset();
                 yellow();
                 printf("\n\n_______________________________________________________________________________________________________________\n");
                 reset();
                 // INCREMENT PC
                 interpreter_ProgramCounter++;

                  // UNCOMMENT FOR DEBUG
 }



 void IAND()
 {

               int iValue2 = (int) (Pop());             
               int iValue1 = (int)(Pop());              
               int  iResult=(int)(iValue1 & iValue2);             
                Push(iResult);             
                // INCREMENT PC
                 interpreter_ProgramCounter++;
               // UNCOMMENT FOR DEBUG
 }

 void IDIV()
 {
              int iValue2 = (int) (Pop());            
              int iValue1 = (int)(Pop());            
              int iResult=iValue1/iValue2;
              Push(iResult);        
              // INCREMENT PC
               interpreter_ProgramCounter++;

                 // UNCOMMENT FOR DEBUG

 }
 void  IFEQ()
 {

              interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
             
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     
           
                          
             int  iResult=(int)(Pop());
              if(iResult==0)
              {
             
                signed short signedShort_offsetAddress=(signed short)((unsigned_a << 8) | unsigned_b);
                 interpreter_ProgramCounter--;
                 interpreter_ProgramCounter--;   
                 interpreter_ProgramCounter=interpreter_ProgramCounter+signedShort_offsetAddress;              
                   
              }
              else
              {
                  //INCREMENT PC
                   interpreter_ProgramCounter++; 
              }   

              // UNCOMMENT FOR DEBUG
 }
 void IFLT()
 {
        

              interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     
           
              
             int  iResult=(int)(Pop());             

              if(iResult<0)
              {
               
                signed short signedShort_offsetAddress=(signed short)((unsigned_a << 8) | unsigned_b);
                 interpreter_ProgramCounter--;
                 interpreter_ProgramCounter--;              
                
                 interpreter_ProgramCounter=interpreter_ProgramCounter+signedShort_offsetAddress;                
              }
              else
              {
                  //INCREMENT PC
                   interpreter_ProgramCounter++; 
              }   
              // UNCOMMENT FOR DEBUG

 }

 void IFGE()
 {

             interpreter_ProgramCounter++;                  
             uint8_t unsigned_a=(uint8_t)(*interpreter_ProgramCounter);  
              interpreter_ProgramCounter++;              
             uint8_t  unsigned_b=(uint8_t)(*interpreter_ProgramCounter);             
              
             int iResult=(int)(Pop()); 
              if(iResult>=0)
              {
                
                signed short  signedShort_offsetAddress=(signed short)((unsigned_a << 8) | unsigned_b);
                 interpreter_ProgramCounter--;
                 interpreter_ProgramCounter--;                                                 
                 interpreter_ProgramCounter=interpreter_ProgramCounter+signedShort_offsetAddress;                
                   
              }
              else
              {
                  //INCREMENT PC
                   interpreter_ProgramCounter++; 
              }   
             // UNCOMMENT FOR DEBUG

 }

 void  IFLE()
{
 
               interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
             
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     
            
              
              int iResult=(int)(Pop());           
              
              if(iResult<=0)
              {
               
                 signed short  signedShort_offsetAddress=(signed short)((unsigned_a << 8) | unsigned_b);
                 interpreter_ProgramCounter--;
                 interpreter_ProgramCounter--;   
                 interpreter_ProgramCounter=interpreter_ProgramCounter+signedShort_offsetAddress;                
                   
              }
              else
              {
                  //INCREMENT PC
                   interpreter_ProgramCounter++; 
              }   
             // UNCOMMENT FOR DEBUG
}

void IFNULL()
{

               interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
             
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     
              
             int iResult=(int)(Pop());         
              
              if(iResult==0)
              {                
                 signed short signedShort_offsetAddress=(signed short)((unsigned_a << 8) | unsigned_b);
                 interpreter_ProgramCounter--;                 
                 interpreter_ProgramCounter--;  

                 interpreter_ProgramCounter=interpreter_ProgramCounter+signedShort_offsetAddress;                
                                  
              }
              else
              {
                  //INCREMENT PC
                   interpreter_ProgramCounter++; 
              }   
              // UNCOMMENT FOR DEBUG

}
//The result is the 32 low-order bits of the true mathematical result in a sufficiently wide two's-complement format, represented as a value of type int. If overflow occurs, then the sign of the result may not be the same as the sign of the mathematical sum of the two values.
void  IMULL()
{
             int iValue2 = (int) (Pop());            
             int iValue1 = (int)(Pop()); 
         
              int iResult=iValue1* iValue2;          
              Push(iResult);
              
              //INCREMENT PC
               interpreter_ProgramCounter++;

            // UNCOMMENT FOR DEBUG

}
void INEG()
{
             int iValue1 = (int) (Pop()); 
             int iResult=-iValue1;
           
              Push(iResult);             

              //INCREMENT PC
              interpreter_ProgramCounter++;
              // UNCOMMENT FOR DEBUG

}
void IOR()
{
               int iValue2 = (int) (Pop());           
               int iValue1 = (int)(Pop()); 
           
               int iResult=(int)(iValue1 | iValue2);              
                
               Push(iResult);  
                   
                //INCREMENT PC
                interpreter_ProgramCounter++;

                // UNCOMMENT FOR DEBUG
               

}

void POP_OperandStack() 	                              // discard the top value on the stack
{
    // Check if StackPointer value is not Less than equal to FramePointer
     if(operandStack_Pointer<=stackFramePointer_Dynamic)
      {
        printf("\n Can not pop as Operand Stack is Empty.");      
      }
     else{
          //int poppedValue=*operandStack_Pointer;   
          operandStack_Pointer--;          
        }                   
              interpreter_ProgramCounter++;
             // UNCOMMENT FOR DEBUG
}


/*
return
Operation
   Return void from method
Format
    return
Forms
    return = 177 (0xb1)
Operand Stack
   ... ??
   [empty]
Description
      The current method must have return type void. If the current method is a synchronized method, 
      the monitor entered or reentered on invocation of the method is updated and possibly exited as if by execution of 
      a monitorexit instruction (§?monitorexit) in the current thread. If no exception is thrown, any values on the operand 
      stack of the current frame (§?2.6) are discarded.
      The interpreter then returns control to the invoker of the method, reinstating the frame of the invoker.
*/
void RETURN()
{
 
        // If it is main method, exit from the program  
        // CHECKING IF THE CURRENT STACK FRAME POINTER IS SAME AS FIRST STACK FRAME POINTER OF MAIN METHOD 
        if(stackFrame_Pointer_In_MainMethod==stackFramePointer_Static) 
        {    yellow();
             printf("\t(Returning from Main Method)\n\n");
             reset();
             exit(0);// RETURING FROM MAIN  
        }
        else
        {
              // RETURN WILL DO THAT SAME AS IRETURN EXCEPT THE RETURNING ANY VALUE FROM PREVIOUS METHOD
     
               
              // Need to take value from current Stack Frame to go to the previous Frame
              // This will be opposite to Invoke Static where we create a new frame and go to that frame
              // We will return from that frame specified by the addresses previously saved by the Invoke Static Method.

              // Read all the value saved in the current Frame
              // First: We need program counter to return to the previous Frame
              // Previous program counter address is saved in the curren frame
             // Always get the physical addres before doing 
             
             

                // get the previous Program counter pointer, this is integer pointer
                int * previousFrame_ProgramCounter_POINTER_TO_PHYSICAL_ADDRESS=Convert_To_PhysicalAddress((uint32_t)(*(stackFramePointer_Static+1)));  
             
                // change it to the unsigned char Pointer
                unsigned char * previousFrame_ProgramCounter_CHAR_POINTER=(unsigned char *)previousFrame_ProgramCounter_POINTER_TO_PHYSICAL_ADDRESS;

              //Get the pointer to the the Physical Address of OPERAND STACK
              int * previouFrame_OperandStack_POINTER_TO_PHISICAL_ADDRESS=Convert_To_PhysicalAddress((uint32_t)(*(stackFramePointer_Static+2)));  

              // Get the pointer to the physical address of the STACK FRAME
              int * previouFrame_StackFrame_POINTER_TO_PHISICAL_ADDRESS=Convert_To_PhysicalAddress((uint32_t)(*(stackFramePointer_Static+3))); 


                // Here operand stack top will be 1 less than the Current Stack Frame
                // NEW OPERAND STACK POINTER
                operandStack_Pointer=stackFramePointer_Static-1; 

              // New Stack Frame Pointer
               stackFramePointer_Static=previouFrame_StackFrame_POINTER_TO_PHISICAL_ADDRESS;
             
             // Once we get the StackFrame Pointer of previous Frame, we can calcuale the other pointers based on the relative postion
              stackFramePointer_Dynamic=stackFramePointer_Static+19;

              // we can now return the pointer for stackFrame_localVariableIndexCounter
               stackFrame_localVariableIndexCounter=stackFramePointer_Static+4;

              // New PROGRAM COUNTER POINTER.
              interpreter_ProgramCounter=previousFrame_ProgramCounter_CHAR_POINTER;       
          
        }       
          
   // PrintStackFrame();         
}



void IRETURN()  //Return int from method
{
       
            // Save the value popped from the Operand Stack to the Return Value Section of the Current Stack Frame
            // Later this will become top of stack of previous Operand Stack
               int iResult=(int)(Pop());
              // Saving the value at the top of the stack frame
              *stackFramePointer_Static=iResult; 

            
              // get the previous Program counter pointer, this is integer pointer
                int * previousFrame_ProgramCounter_POINTER_TO_PHYSICAL_ADDRESS=Convert_To_PhysicalAddress((uint32_t)(*(stackFramePointer_Static+1)));  
             
                // change it to the unsigned char Pointer
                unsigned char * previousFrame_ProgramCounter_CHAR_POINTER=(unsigned char *)previousFrame_ProgramCounter_POINTER_TO_PHYSICAL_ADDRESS;
               
               // PROGRAM COUNTER
                interpreter_ProgramCounter=previousFrame_ProgramCounter_CHAR_POINTER;
               
              //PREVIOUS OPERAND STACK POINTER
              int * previouFrame_OperandStack_POINTER_TO_PHISICAL_ADDRESS=Convert_To_PhysicalAddress((uint32_t)(*(stackFramePointer_Static+2)));  

             // PREVIOUS STACK FRAME POINTER
              int * previouFrame_StackFrame_POINTER_TO_PHISICAL_ADDRESS=Convert_To_PhysicalAddress((uint32_t)(*(stackFramePointer_Static+3))); 


                // PREVIOUS OPERAND STACK IS THE SAME AS CURRENT STACK FRAME ADDRESS
                operandStack_Pointer=stackFramePointer_Static;                
              
              // STACK FRAME POINTER STATIC
               stackFramePointer_Static=previouFrame_StackFrame_POINTER_TO_PHISICAL_ADDRESS;
             
             // DYNAMIC FRAME POINTER
              stackFramePointer_Dynamic=stackFramePointer_Static+19;

              // LOCAL VARIABLE POINTER
               stackFrame_localVariableIndexCounter=stackFramePointer_Static+4;
           
            //PrintStackFrame(); // UNCOMMENT FOR DEBUG   
           //printf("\n");

}

/*

invokestatic
Operation
      Invoke a class (static) method
Format
        invokestatic
        indexbyte1
        indexbyte2
Forms
      invokestatic = 184 (0xb8)

Operand Stack
      ..., [arg1, [arg2 ...]] ??.     ..

Description
        The unsigned indexbyte1 and indexbyte2 are used to construct an index into the run-time constant pool of the current class (§?2.6), where the value of the index is (indexbyte1 << 8) | indexbyte2. 
        The run-time constant pool item at that index must be a symbolic reference to a method (§?5.1), 
        which gives the name and descriptor (§?4.3.3) of the method as well as a symbolic reference to the class in which the method is to be found. The named method is resolved (§?5.4.3.3). 
        The resolved method must not be an instance initialization method (§?2.9) or the class or interface initialization method (§?2.9). It must be static, and therefore cannot be abstract.

        On successful resolution of the method, the class that declared the resolved method is initialized (§?5.5) if that class has not already been initialized.
        The operand stack must contain nargs argument values, where the number, type, and order of the values must be consistent with the descriptor of the resolved method.


*/

void INVOKE_STATIC()
{
  //printf("\n before creating a frame");
  //PrintStackFrame();

             
             
              interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
              
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     

             
              int16_t constnatPool_Reference=(int16_t)(unsigned_a << 8 | unsigned_b); 

              unsigned char * referenceToNewMethod=(unsigned char *)(constantPool_Pointer_VM+constnatPool_Reference*7);          
              uint32_t methodReference = ((uint32_t)referenceToNewMethod[0] << 24) | ((uint32_t)referenceToNewMethod[1] << 16) | ((uint32_t)referenceToNewMethod[2] << 8) | (uint32_t)referenceToNewMethod[3];
              
              
              yellow();
              printf("\n\t\t\t Method reference for jump : %d",methodReference);             
              reset();

              // This is for Diagnostic Purpose. It highlights the byte in Exec
              // Do not delete

              //Highlight_ProgramByteCode(methodReference);

              // This is for Diagnostic Purpose. It highlights the byte in cosntant pool
              // Do not delete
             // Highlight_ConstantPoolReference(referenceToNewMethod);

             // Create a New Frame
              uint8_t  numberOfArguments_InvokeStatic=(uint8_t )(*(referenceToNewMethod+4));
              uint8_t  argumentType_InvokeStatic=(uint8_t )(*(referenceToNewMethod+5));
              uint8_t  returnValue_InvokeStatic=(uint8_t )(*(referenceToNewMethod+6)); 
 
            // SINCE WE HAVE READ TWO BYTES BY PROGRAM COUNTER TO CREATE THE INDEX, NEED TO DECREMENT THE PC TO GO THE OPCODE
            interpreter_ProgramCounter--;
            interpreter_ProgramCounter--;

             // WE NEED TO PASS THE ARGEMENT TO THE 
             int arraryOfArgument[16]={0};
             int *argumentPointer_InvokeStatic=arraryOfArgument;  
             int poppedValue;
             int argCounter;
            // POP THE VALUE AND THEN SAVE IN THE ARRAY
              for(argCounter=numberOfArguments_InvokeStatic-1;argCounter>=0;argCounter--)
              {                
                   poppedValue=(int) (Pop());                
                   arraryOfArgument[argCounter]=poppedValue;                 
              }

          // SAVE THE VALUES INTO THE CURRENT FRAME
          *(operandStack_Pointer+1)=returnValue_InvokeStatic;                                               // LOCATION 0: RETURN VALUE              
          *(operandStack_Pointer+2)=(uint32_t)(Convert_To_LogicalAddres(interpreter_ProgramCounter+3));     // LOCATION 1: PREVIOUS PROGRAM COUNTER
          *(operandStack_Pointer+3)=(uint32_t)(Convert_To_LogicalAddres(operandStack_Pointer));              //  (uint32_t)((long)operandStack_Address_InvokeStatic);         // LOCATION 2: PREVIOUS OPERAND STACK ADDRESS
          *(operandStack_Pointer+4)=(uint32_t)(Convert_To_LogicalAddres(stackFramePointer_Static));         // LOCATION 3: PREVIOUS FRAME ADDRESS

         // Change the Pointer Variables
         stackFramePointer_Static= operandStack_Pointer + 1;                      // new stack frame pointer STATIC
         stackFramePointer_Dynamic=stackFramePointer_Static+19;                   // new stack frame pointer DYNAMIC
         stackFrame_localVariableIndexCounter = stackFramePointer_Static + 4;     // new index to the local variables
         operandStack_Pointer = stackFramePointer_Static + 19;                     // new Oprand stack . It will be pointing to the Top-1    


        // Copying the previous values to the new Local variable Location         // LOCTION  4: SAVE THE PARAMETER       
         for(argCounter=0;argCounter<=numberOfArguments_InvokeStatic-1;argCounter++)
         {
            *(stackFrame_localVariableIndexCounter+argCounter)=*(argumentPointer_InvokeStatic+argCounter);       
         }
          // INTERPRETE PC WILL BE JVM + THE REFERENCE BY THE CONSTANT POOL
          interpreter_ProgramCounter = (unsigned char *)JVM+methodReference;


           //header="INSIDE INVOKE STATIC: Created a New Frame";
          // Printheader(header);
           //PrintStackFrame(); // UNCOMMENT FOR DEBUG
}
void IREAD()
{
          int readInt;
          scanf("%d",&readInt);              
          Push(readInt);
         // INCREMENT PC
          interpreter_ProgramCounter++;

        // PrintStackFrame(); // UNCOMMENT FOR DEBUG
}

/*
new
Operation
    Create new object
Format
    new
    indexbyte1
    indexbyte2
Forms
    new = 187 (0xbb)
Operand Stack
.   .. ??
    ..., objectref
Description
    The unsigned indexbyte1 and indexbyte2 are used to construct an index into the run-time constant pool of the current class (§?2.6), 
    where the value of the index is (indexbyte1 << 8) | indexbyte2. The run-time constant pool item at the index must be a 
    symbolic reference to a class or interface type. The named class or interface type is resolved (§?5.4.3.1) 
    and should result in a class type. Memory for a new instance of that class is allocated from the garbage-collected heap, 
    and the instance variables of the new object are initialized to their default initial values (§?2.3, §?2.4). The objectref, 
    a reference to the instance, is pushed onto the operand stack.
*/
void NEW()    
{ 
               interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
             
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     

              // Create new object of type identified by class reference in constant pool index (indexbyte1 << 8 | indexbyte2)
               int16_t index_in_constPool=(int16_t)(unsigned_a << 8 | unsigned_b); 

             
            // Unsigned char because the constnat pool file itself has one byte values
             unsigned char *fieldReference_In_Const_Pool=(unsigned char *)(objectReference_In_ConstantPool+index_in_constPool*4);  

            // Highlight_ConstantPoolReference(fieldReference_In_Const_Pool);           
             
             // Get the Size of Fields Pointed by the fieldReference_In_Const_Pool 
               uint8_t  numberOfObjects=(uint8_t )(*(fieldReference_In_Const_Pool));  

             // Need to save the pointer to the stack   // Push the START OF THE logical address into the stack             
              uint32_t logicalAddress_HeapPointer=(uint32_t)(Convert_To_LogicalAddres(heapPointer_VM));                 
               Push(logicalAddress_HeapPointer);    
             
             // Assign zero to all objects in the heap  
              int COUNTER=0;                               
              for(COUNTER=0;COUNTER<numberOfObjects;COUNTER++)
              {
                 *(heapPointer_VM-COUNTER)=0; 
                 yellow();
                 printf("\n\t\t\t Object Created at Address: %ld (%p)",(heapPointer_VM-COUNTER),(heapPointer_VM-COUNTER));
                 reset();
              }                                            
             
                heapPointer_VM=heapPointer_VM-numberOfObjects;     // Decrement the heap Pointer based on the size of the fields              
                interpreter_ProgramCounter++;    // Increment the Program Counter
               // PrintStackFrame(); // UNCOMMENT FOR DEBUG               

}


/*
anewarray
Operation
    Create new array of reference
Format
    anewarray
    indexbyte1
    indexbyte2
Forms   
anewarray = 189 (0xbd)
Operand Stack
    ..., count ??
    ..., arrayref
Description
    The count must be of type int. It is popped off the operand stack. 
    The count represents the number of components of the array to be created.
     The unsigned indexbyte1 and indexbyte2 are used to construct an index into the run-time constant pool of the current class (§?2.6), 
     where the value of the index is (indexbyte1 << 8) | indexbyte2. The run-time constant pool item at that index must be a 
     symbolic reference to a class, array, or interface type. The named class, array, or interface type is resolved (§?5.4.3.1). 
     A new array with components of that type, of length count, is allocated from the garbage-collected heap,
      and a reference arrayref to this new array object is pushed onto the operand stack. 
      All components of the new array are initialized to null, the default value for reference types (§?2.4).
*/
void ANEWARRAY()  //Create new array of reference
{
    
              int number_of_components_of_array_to_be_Created=(int)(Pop());   
         
            
              interpreter_ProgramCounter++;  
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
             
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     


              //Get the Index in Constant Pool               
               int16_t index_In_ConstantPool=(int16_t)(unsigned_a << 8 | unsigned_b); 
               
               // Get the Pointer
              unsigned char * symbolic_reference_To_Array=(unsigned char *)(objectReference_In_ConstantPool+index_In_ConstantPool*4);  
              
                // Need to create an array and save the starting address into the stack
                  // Relative Address
                 uint32_t logicalAddress_HeapPointer=(uint32_t)(Convert_To_LogicalAddres(heapPointer_VM));
                
                 // Push the Logical Address into stack           
                 Push(logicalAddress_HeapPointer);

                  int COUNTER=0;             
                 for(COUNTER=0;COUNTER<number_of_components_of_array_to_be_Created;COUNTER++)
                  {
                     *((heapPointer_VM)-COUNTER)=0; 
                   }                     
            
             
                  // Decrement the Heap Pointer
                  heapPointer_VM=heapPointer_VM-number_of_components_of_array_to_be_Created;
                 
                 // INCREMENT PC
                  interpreter_ProgramCounter++;  
               // PrintStackFrame(); // UNCOMMENT FOR DEBUG

}


/*
newarray
Operation
    Create new array
Format
    newarray
    atype
Forms
    newarray = 188 (0xbc)
Operand Stack
.   .., count ??
  ..., arrayref
Description
    The count must be of type int. It is popped off the operand stack. 
    The count represents the number of elements in the array to be created.
    The atype is a code that indicates the type of array to create. It must take one of the following values:
*/
void NEWARRAY()
{
     
              int totalSpaceForArray,i,loopCounter;
              // We need to declare different Pointers to point to different types of objects
              uint8_t *heapPointer_Type_Bool,*heapPointer_Type_Char,*heapPointer_Type_Byte;
              uint16_t *heapPointer_Type_Short;
              uint32_t *heapPointer_Type_Float,*heapPointer_Type_Long,*heapPointer_Type_Int;
              uint64_t *heapPointer_Type_Double;

             // Get the Array Type
              interpreter_ProgramCounter++;             
             // uint8_t arrayType=(uint8_t)(*interpreter_ProgramCounter); 
              uint8_t arrayType=(uint8_t)((unsigned char)*interpreter_ProgramCounter);  


              // Get the Count of Elements of Array           
               int numberOfItems=(int)(Pop());       
              
              // Reseting the value to the Default Value
               if(arrayType==4)                                  //  T_BOOLEAN	4
               {
                  // Reserve the space in the Heap according to the Type of the item
                 totalSpaceForArray=numberOfItems*1;              // Boolean reserves 1 Byte
                 heapPointer_Type_Bool=(uint8_t *)heapPointer_VM;
                 for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
              int totalSpaceForArray,i,loopCounter;
                 *(heapPointer_Type_Bool-loopCounter)=false;               // Default Value of Bool=false
                 } 
               }
              else if(arrayType==5)                              // T_CHAR	5
              {
                  // Reserve the space in the Heap according to the Type of the item
                 totalSpaceForArray=numberOfItems*1;               // Char reserves 1 Byte
                 heapPointer_Type_Char=(uint8_t *)heapPointer_VM;
                 for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                  {
                    *(heapPointer_VM-loopCounter)='\0';                     // Default Value of Bool=false
                  }  
              }
              else if(arrayType==6)                               // T_FLOAT	6
              {
                // Reserve the space in the Heap according to the Type of the item
                 totalSpaceForArray=numberOfItems*4;              // Float reserves 4 Byte in 32 bit computer
                 heapPointer_Type_Float=(uint32_t *)heapPointer_VM;
                  for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
                 *(heapPointer_Type_Float-loopCounter)=0.0f;         // Default Value of Float 0.0f
                 }  
              }
            else if(arrayType==7)                           // T_DOUBLE	7
            {
                // Reserve the space in the Heap according to the Type of the item
                 totalSpaceForArray=numberOfItems*8;               // Double reserves 8 Byte in 32 bit computer
                 heapPointer_Type_Double=(uint64_t *)heapPointer_VM;
                 for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
                  *(heapPointer_Type_Double-loopCounter)=0.0;          // Default Value of Double 0.0
                 } 
            }
            else if(arrayType==8)                            // T_BYTE	8
            {
                // Reserve the space in the Heap according to the Type of the item
                 totalSpaceForArray=numberOfItems*1;           // Byte reserves 1 Byte in 32 bit computer
                 heapPointer_Type_Byte=(uint8_t *)heapPointer_VM;
                for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
                 *(heapPointer_Type_Byte-loopCounter)=0;                      
                 
                }  
            }
            else if(arrayType==9)                            // T_SHORT	9              
            {
                 totalSpaceForArray=numberOfItems*2;          // Short reserves 2 Byte in 32 bit computer
                 heapPointer_Type_Short=(uint16_t *)heapPointer_VM;
                for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
                 *(heapPointer_Type_Short-loopCounter)=0; 
                }                  
            } 
           else if(arrayType==10)                            // T_INT	10                
            {
                // Reserve the space in the Heap according to the Type of the item
                 totalSpaceForArray=numberOfItems*4;
                 heapPointer_Type_Int=(int *)heapPointer_VM;                 
                 for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
                 *(heapPointer_Type_Int-loopCounter)=0; 
                }                
            } 
           else if(arrayType==11)                            // T_LONG	11                  
            {
                // Reserve the space in the Heap according to the Type of the item
                totalSpaceForArray=numberOfItems*4;
                heapPointer_Type_Long=(int *)heapPointer_VM;  
                for(loopCounter=0;loopCounter<numberOfItems;loopCounter++)
                 {
                 *(heapPointer_Type_Long-loopCounter)=0; 
                }           
            }                 
                
                //Need to save the pointer to the stack
                uint32_t logicalAddress_HeapPointer=(uint32_t)(Convert_To_LogicalAddres(heapPointer_VM));  
               
                // Pushing the referece to the array            
                Push(logicalAddress_HeapPointer);
                
                // New heap Pointer
                heapPointer_VM=heapPointer_VM-totalSpaceForArray;

                // INCREMENT PC
                interpreter_ProgramCounter++;     

             //  PrintStackFrame(); // UNCOMMENT FOR DEBUG  

}

  /*
getfield
Operation
             Fetch field from object
Format
              getfield
              indexbyte1
              indexbyte2
Forms
            getfield = 180 (0xb4)
Operand Stack
        ..., objectref ??
          ..., value  
Description
          The objectref, which must be of type reference, is popped from the operand stack. 
          The unsigned indexbyte1 and indexbyte2 are used to construct an index into the run-time constant pool of the current class (§?2.6), 
          where the value of the index is (indexbyte1 << 8) | indexbyte2. 
          The run-time constant pool item at that index must be a symbolic reference to a field (§?5.1), which gives the name and descriptor of the field as well as a symbolic reference to the class in which the field is to be found. 
          The referenced field is resolved (§?5.4.3.2). The value of the referenced field in objectref is fetched and pushed onto the operand stack.

          The type of objectref must not be an array type. 
          If the field is protected (§?4.6), and it is a member of a superclass of the current class, 
          and the field is not declared in the same run-time package (§?5.3) as the current class, 
          then the class of objectref must be either the current class or a subclass of the current class.

Linking Exceptions
          During resolution of the symbolic reference to the field, any of the errors pertaining to field resolution (§?5.4.3.2) can be thrown.
          Otherwise, if the resolved field is a static field, getfield throws an IncompatibleClassChangeError.

Run-time Exception
          Otherwise, if objectref is null, the getfield instruction throws a NullPointerException.


*/
     

void GETFIELD()  
{ 

             //The objectref, which must be of type reference, is popped from the operand stack. 
            int objectref=(int)(Pop());
            uint32_t objectref_unsigned=(uint32_t)(objectref); //   CASTING TO UNSIGNED INT AS OBJECT REF MUST BE POSTIVE 

              //The value of the index is (indexbyte1 << 8) | indexbyte2.
              interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     

 
             //get the details of the method of the  
              int16_t index_In_Object=(int16_t)(unsigned_a << 8 | unsigned_b);             
             
              //The run-time constant pool item at that index must be a symbolic reference to a field 
             // unsigned char * symbolic_reference_To_Field=(unsigned char *)((objectReference_In_ConstantPool+(index_In_ConstantPool*4))); 
                        
              //VALUE AT POINTER: Get value at Pointer
              //uint32_t fieldReferenceIndex;
              //fieldReferenceIndex=(uint32_t)((unsigned char)*symbolic_reference_To_Field); 
              
                      

            int * physicalAddress_Of_Object=Convert_To_PhysicalAddress(objectref_unsigned); 
            int * field_Referenced_Pointer=(int *)(physicalAddress_Of_Object-(index_In_Object));
            
            
          
           // printf("\n\t\t\t Value %u is Retrieved from Address: %ld (%p) and pushing at the Top of Stack at %ld",*field_Referenced_Pointer,field_Referenced_Pointer,field_Referenced_Pointer,operandStack_Pointer);
         


             // Get the Value from this address
             int value_of_Field=*(field_Referenced_Pointer);
             
              // Push the Field value back to the stack            
               Push(value_of_Field);
           
              // Increment the PC
             interpreter_ProgramCounter++;
          //   PrintStackFrame(); // UNCOMMENT FOR DEBUG
}

/*

putfield
Operation
     Set field in object
Format
      putfield
       indexbyte1
       indexbyte2
Forms
     putfield = 181 (0xb5)

Operand Stack
.    .., objectref, value ??...

Description
      The unsigned indexbyte1 and indexbyte2 are used to construct an index into the run-time constant pool of the current class (§?2.6), 
      where the value of the index is (indexbyte1 << 8) | indexbyte2. The run-time constant pool item at that index must be a symbolic reference to a field (§?5.1), which gives the name and descriptor of the field as well as a symbolic reference to the class in which the field is to be found. 
      The class of objectref must not be an array. 
      If the field is protected (§?4.6), and it is a member of a superclass of the current class, 
      and the field is not declared in the same run-time package (§?5.3) as the current class, 
      then the class of objectref must be either the current class or a subclass of the current class.

*/

void PUTFIELD() //set field to value in an object objectref, where the field is identified by a field reference index in constant pool (indexbyte1 << 8 | indexbyte2)
{
  
              interpreter_ProgramCounter++;             
              uint8_t unsigned_a=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     //reference: (uint32_t)((signed char)*interpreter_ProgramCounter); 
              interpreter_ProgramCounter++;             
              uint8_t unsigned_b=(uint8_t)((unsigned char)*interpreter_ProgramCounter);     

             // interpreter_ProgramCounter++;             
            //  uint8_t unsigned_a=(uint8_t)(*interpreter_ProgramCounter);  
             // interpreter_ProgramCounter++;             
             // uint8_t unsigned_b=(uint8_t)(*interpreter_ProgramCounter);   

              // Index in Constant Pool            
              int16_t index_In_Object=(int16_t)(unsigned_a << 8 | unsigned_b); 
              // printf("\n index_In_ConstantPool %d",index_In_Object);
               // Pointer to the Constant Pool
             //unsigned char * symbolic_reference_To_Field=(unsigned char *)(objectReference_In_ConstantPool+(index_In_ConstantPool*4)); 

         
             //VALUE AT POINTER: Get value at Pointer
             // uint32_t fieldReferenceIndex;
              //fieldReferenceIndex=(uint32_t)((unsigned char)*symbolic_reference_To_Field); 
              
              // //Get value at Pointer
              // uint8_t fieldReferenceIndex=(uint8_t)(*(objectReference_In_Const_Pool)); 

              // Retrieve two values from the top of the stack.             
                int value=(int)(Pop());             //First one is the Value      
                int objectref=(int)(Pop());         //Second  one is the Object Reference     
                uint32_t objectref_UnsignedIntFormat= (uint32_t) objectref; // Object Reference can not be negative number       

            // Go the the field referenced by sizeOfObject. We need to go to the actual location               
            int* physicalAddress_Of_Object=Convert_To_PhysicalAddress(objectref_UnsignedIntFormat);
     
             // Point to the actual address in the heap where the objects are created
             int *object_Referenced=(int *)(physicalAddress_Of_Object-(index_In_Object));
          
             // printf("\n object_Referenced=%u %p",object_Referenced,object_Referenced);
             // SAVE THE VALUE TO THE PHYSICAL ADDRESS 
          
            // printf("\n\t\t\tPutting Value : %u at Address : %ld (%p) ",value,object_Referenced,object_Referenced);
         
             *(object_Referenced)=value;

            // Increment the PC
             interpreter_ProgramCounter++;

        // PrintStackFrame(); // UNCOMMENT FOR DEBUG
}




/*
Operation
         Load int from local variable
Format
     iload
    index
Forms
      iload = 21 (0x15)
Description: 
      The index is an unsigned byte that must be an index into the local variable array of the current frame (§?2.6).
       The local variable at index must contain an int. 
       The value of the local variable at index is pushed onto the operand stack.
*/
void ILOAD()
{
     interpreter_ProgramCounter++;                 
     uint8_t index =(uint8_t)(*interpreter_ProgramCounter);
     int iResult=(int)(*(stackFrame_localVariableIndexCounter+index));    // Go to the Index of the local variables tables and save the value
           
     //printf("\n\t\t\t Load (Int) Value: {%u} from index [%d] . Address: %ld",*(stackFrame_localVariableIndexCounter+index),index,(stackFrame_localVariableIndexCounter+index));      
           
      


      Push(iResult);  
      interpreter_ProgramCounter++;    // INCREMENT PC
                    // UNCOMMENT FOR DEBUG

}





/*
 Operation
     Load int from array
Format
     iaload
Forms
      iaload = 46 (0x2e)
Operand Stack
    ..., arrayref, index ?? 
    ..., value
Description
       The arrayref must be of type reference and must refer to an array whose components are of type int. 
       The index must be of type int. Both arrayref and index are popped from the operand stack. 
       The int value in the component of the array at index is retrieved and pushed onto the operand stack.
Run-time Exceptions
      If arrayref is null, iaload throws a NullPointerException.
      Otherwise, if index is not within the bounds of the array referenced by arrayref, the iaload instruction throws an ArrayIndexOutOfBoundsException.
*/
 void IALOAD()
 {         

                int index_To_Array = (int)(Pop());               
                int  arrayref = (int)(Pop());     
                if(arrayref==0)// If arrayref is null, iaload throws a NullPointerException.
                {
                    red();
                     printf("\nArray Reference is Null");
                     exit(EXIT_FAILURE);
                }         
               // Since the Index and Array Reference cannot be negative number, 
               // Changing their type to unsigned Inteter

                uint32_t index_To_Array_Unsigned=(uint32_t) index_To_Array;               
                uint32_t arrayref_Unsigned= (uint32_t) arrayref;

              // printf("\n\t\t\t index_To_Array_Unsigned=%u",index_To_Array_Unsigned);
              // printf("\n\t\t\t arrayref Logical Address=%u",arrayref_Unsigned);

                int * physicalAddress_Of_Array=Convert_To_PhysicalAddress(arrayref_Unsigned);    
               


                int iResult=(int)(*(physicalAddress_Of_Array+index_To_Array_Unsigned));  
              // printf("\n\t\t\t Load Int Value: {%u} from Index [%d] . Address: (%ld)",iResult,index_To_Array_Unsigned,(physicalAddress_Of_Array+index_To_Array_Unsigned));         
               
               
                Push(iResult);                      
               // INCREMENT PC
                interpreter_ProgramCounter++;
                // UNCOMMENT FOR DEBUG
 }



/*

aload
Operation
     Load reference from local variable
Format
     aload
     index
Forms
    aload = 25 (0x19)

Operand Stack
.     .. ??
.     .., objectref

Description
       The index is an unsigned byte that must be an index into the local variable array of the current frame (§?2.6). 
       The local variable at index must contain a reference. 
       The objectref in the local variable at index is pushed onto the operand stack.

Notes
The aload instruction cannot be used to load a value of type returnAddress from a local variable onto the operand stack. This asymmetry with the astore instruction (§?astore) is intentional.

The aload opcode can be used in conjunction with the wide instruction (§?wide) to access a local variable using a two-byte unsigned index.
*/
 void ALOAD()               
 {

             interpreter_ProgramCounter++;             
             uint8_t index_To_LocalVariable=(uint8_t)(*interpreter_ProgramCounter);            
          
             // Object reference should not be a negative number, so changing to unsigned int    
             //printf("\n stackFrame_localVariableIndexCounter address %ld (%p) value : %u",stackFrame_localVariableIndexCounter,stackFrame_localVariableIndexCounter,*stackFrame_localVariableIndexCounter); 
            
  
             
             uint32_t object_In_StackFrame =(uint32_t)(*(stackFrame_localVariableIndexCounter+index_To_LocalVariable));   
            
          // printf("\n\t\t\t Load Reference {%u} from index [%d] . Address: %ld",object_In_StackFrame,index_To_LocalVariable,(stackFrame_localVariableIndexCounter+index_To_LocalVariable));      
            
             Push(object_In_StackFrame);               // PUSH it to the Operand Stack              
             interpreter_ProgramCounter++;           // Increnent the PC

               // UNCOMMENT FOR DEBUG
 }




/*
aaload 
Operation
    Load reference from array
Format
    aaload
Forms
    aaload = 50 (0x32)

Operand Stack
..., arrayref, index ??
..., value

Description
     The arrayref must be of type reference and must refer to an array whose components are of type reference. 
     The index must be of type int. Both arrayref and index are popped from the operand stack.
      The reference value in the component of the array at index is retrieved and pushed onto the operand stack.

Run-time Exceptions
     If arrayref is null, aaload throws a NullPointerException.

*/

void AALOAD()  //load onto the stack a reference from an array
{
                // Get the Index
                int index_To_Array = (int) (Pop()); 
           
                // Get the Array refernce
                int  arrayref = (int)(Pop());
                uint32_t arrayref_Signed= (uint32_t) arrayref;

                //If arrayref is null, aaload throws a NullPointerException.
               //Otherwise, if index is not within the bounds of the array referenced by arrayref, the aaload instruction throws an ArrayIndexOutOfBoundsException.
              if(arrayref==0)
              {
                 perror("NullPointerException");
                  exit(EXIT_FAILURE);
              }
              else
              {
                           
                
                int * physicalAddress_Of_Array_Pointer=Convert_To_PhysicalAddress(arrayref_Signed);              
                uint32_t iResult=(uint32_t)(*(physicalAddress_Of_Array_Pointer+index_To_Array)); 
               // Result is pushed onto the stack                       
                Push(iResult);     
              }             
                       
              
               //INCREMENT PC
                interpreter_ProgramCounter++;
           
            
              
 // PrintStackFrame(); // UNCOMMENT FOR DEBUG
}

/*

dup
Operation
      Duplicate the top operand stack value
Format
      dup
Forms
    dup = 89 (0x59)
Operand Stack
    ..., value ??
    ..., value, value
Description
      Duplicate the top value on the operand stack and push the duplicated value onto the operand stack.
      The dup instruction must not be used unless value is a value of a category 1 computational type (§?2.11.1).
*/
void DUP()   //duplicate the value on top of the stack
{
             
               int valueToDuplicte=(int)(Pop());     // Popping the value which needs to be duplicated                          
               Push(valueToDuplicte);               // Push the value twice into the stack for duplication  
               Push(valueToDuplicte);                // PUSH again the same object
	             interpreter_ProgramCounter++;           // Increment the PC  

             // UNCOMMENT FOR DEBUG

}


/*
aconst_null
Operation
    Push null
Format
    aconst_null
Forms
    aconst_null = 1 (0x1)
Operand Stack
    ... ??
  ..., null
Description
    Push the null object reference onto the operand stack.
Notes
    The Java Virtual Machine does not mandate a concrete value for null.
*/
void ACONST_NULL()   //	push a null reference onto the stack
{
   Push(0);
   interpreter_ProgramCounter++;     // INCREMENT PC
   // UNCOMMENT FOR DEBUG
}


/*
Operation
    Store reference into local variable
Format
     astore
     index
Forms
     astore = 58 (0x3a)
Operand Stack
.    .., objectref ??
      ...
Description
     The index is an unsigned byte that must be an index into the local variable array of the current frame (§?2.6). 
     The objectref on the top of the operand stack must be of type returnAddress or of type reference. It is popped from the operand stack, 
     and the value of the local variable at index is set to objectref.

Notes
    The astore instruction is used with an objectref of type returnAddress when implementing the finally clause of the Java programming language (§?3.13).
    The aload instruction (§?aload) cannot be used to load a value of type returnAddress from a local variable onto the operand stack. This asymmetry with the astore instruction is intentional.
    The astore opcode can be used in conjunction with the wide instruction (§?wide) to access a local variable using a two-byte unsigned index.
*/
 void  ASTORE()
 {
            interpreter_ProgramCounter++;             
            uint8_t index_To_LocalVariable=(uint8_t)(*interpreter_ProgramCounter);  
           
              int objectref=(int)(Pop());                                                   // Object Reference is POPPed from the stack  
              // since object ref is a reference type, it should not  be negative value                       
              uint32_t objectReferenece=(uint32_t) objectref;  
                // Save the value on the local variables table        
             (*(stackFrame_localVariableIndexCounter+index_To_LocalVariable))=objectReferenece; 

           //  printf("\n\t\t\t Stored Object Reference: {%u} at the Local Variable Index [%d] at address (%ld)",objectReferenece,index_To_LocalVariable,(stackFrame_localVariableIndexCounter+index_To_LocalVariable));
               interpreter_ProgramCounter++;                                                  // Increment the PC  

           // UNCOMMENT FOR DEBUG
 }



/*
iastore
Operation
      Store into int array
Format
    iastore
Forms
    iastore = 79 (0x4f)
Operand Stack
    ..., arrayref, index, value ??
    ...
Description
    The arrayref must be of type reference and must refer to an array whose components are of type int. 
    Both index and value must be of type int. The arrayref, index, and value are popped from the operand stack. 
    The int value is stored as the component of the array indexed by index.

Run-time Exceptions
    If arrayref is null, iastore throws a NullPointerException.
    Otherwise, if index is not within the bounds of the array referenced by arrayref, the iastore instruction throws an ArrayIndexOutOfBoundsException.

*/
 void IASTORE()
 {

                int value = (int) (Pop());  
                int index_To_Array = (int)(Pop());
                int arrayref = (int)(Pop());
                // Index of Array Must be Postive Number
                uint32_t index_To_Array_unsigned=(uint32_t) index_To_Array;

               //The arrayref must be of type reference and must refer to an array whose components are of type int.
               // Chaning the value to unsigned int as the reference cannot be negative number
                uint32_t arrayref_unsigned= (uint32_t) arrayref;  
                int * physicalAddress_Of_Array=Convert_To_PhysicalAddress(arrayref_unsigned);

                //uint8_t *physicalAddress_Of_Object_8Bit=(uint8_t *)physicalAddress_Of_Object_32Bit; 
                 *(physicalAddress_Of_Array+index_To_Array_unsigned)=value;                
                            
              // INCREMENT PC
               interpreter_ProgramCounter++;

              // UNCOMMENT FOR DEBUG
 }


/*

aastore
Operation
     Store into reference array
Format
      aastore
Forms
      aastore = 83 (0x53)
Operand Stack
     ..., arrayref, index, value ??
.     ..
Description
       The arrayref must be of type reference and must refer to an array whose components are of type reference. 
       The index must be of type int and value must be of type reference. The arrayref, index, and value are popped from the operand stack. 
       The reference value is stored as the component of the array at index.

*/
void AASTORE() //store a reference in an array
{ //PrintStackFrame();
  
               // Got the value
               int referenced_Value = (int) (Pop());  //The reference value is stored as the component of the array at index.
              //This value cannot be negative as it is a referenced Type
              // So change it to unsigned int
              uint32_t referenced_Value_Unsigned=(uint32_t) referenced_Value;

               //Got the Index
                uint32_t index_To_Array = (uint32_t)(Pop());
          

                //Got the Array Ref
                int arrayref = (int)(Pop());
               // Array Reference cannot be a negative number as it an address
                uint32_t arrayref_Unsigned= (uint32_t) arrayref;
                if(arrayref==0)
                {
                  perror("NullPointerException");
                  exit(EXIT_FAILURE);
                }
                else
                {
                    
                     int * physicalAddress_Of_Array_Pointer=Convert_To_PhysicalAddress(arrayref_Unsigned);            
                      *(physicalAddress_Of_Array_Pointer+index_To_Array)=referenced_Value_Unsigned; 
                }                
                           
                            
              // INCREMENT PC
               interpreter_ProgramCounter++;

             // UNCOMMENT FOR DEBUG
}




//____________________________________________________________________________________________________________________________
// END: ALL BYTE CODE INSTRUCTION 
//____________________________________________________________________________________________________________________________


//____________________________________________________________________________________________________________________________
// THIS METHOD WILL PRINT THE OPCODE
//____________________________________________________________________________________________________________________________
PrintOpcode(uint32_t opcode,char *opcode_Name)
{
    red();
    printf("\n OPCODE: %d (0x%x)",opcode,opcode);
    reset();
    green();
    printf(" %s", opcode_Name);
    reset(); 
}


//____________________________________________________________________________________________________________________________
 // This Method should create a Stake Frame and Value Stack on the Top of the Stack
//____________________________________________________________________________________________________________________________
void SaveValueInStackFrame(uint32_t stackFrame_ReturnValue, 
                                        uint32_t programCounter_Address,                      
                                        uint32_t operandStack_Address,
                                        uint32_t stackFrame_Address,
                                        uint32_t numberOfArguments,
                                         int *parameterPointer){
                                                                        
    *stackFramePointer_Static=stackFrame_ReturnValue;                                     // LOCATION 0: RETUNN VALUE
    *(stackFramePointer_Static+1)=(uint32_t)((long)programCounter_Address);               // LOCATION 1: PREVIOUS PROGRAM COUNTER
    *(stackFramePointer_Static+2)=(uint32_t)((long)operandStack_Address);                 // LOCATION 2: PREVIOUS OPERAND STACK ADDRESS
    *(stackFramePointer_Static+3)=(uint32_t)((long)stackFrame_Address);                   // LOCATION 3: PREVIOUS FRAME ADDRESS
  
     int argCounter;
     for(argCounter=0;argCounter<numberOfArguments;argCounter++)
     {
       *(stackFrame_localVariableIndexCounter+argCounter)=*(parameterPointer+argCounter);       
     }
    
   }


//____________________________________________________________________________________________________________________________
// This will validate the user input
//____________________________________________________________________________________________________________________________
bool ValidateInputParameter(char *argv[])
{
   
if(!(strcmp(argv[1], "-e") == 0) ||!(strcmp(argv[3], "-c") == 0)) 
  {
 red();
 printf("\nParameters are not valid\n");
 reset();
 return false;
 }
return true;

}

//____________________________________________________________________________________________________________________________
// This function will get the Length of the Argv
//____________________________________________________________________________________________________________________________
int  argv_length(char** argv)
{
    int count = 0;
    while(argv[++count]);
    return count;
}


//____________________________________________________________________________________________________________________________
// This function will load the File Containing Byte Code from the argument
// Files must the in the same location where our C output file exists
//____________________________________________________________________________________________________________________________
int LoadBinaryFileIntoVM(char *argv[],int argmentPostion,int IndexForCopy,int * JVM)
{

FILE *filePointer;
unsigned char *instuctionStart;
unsigned char *instructionEnd;
unsigned char *program_Pointer_VM;
long fileLength; 
unsigned char *pointerBinaryFile; // This is a pointer to the binay file that will be loaded 
int i; 
// Open the Binary File 
filePointer=fopen(argv[argmentPostion],"rb"); 
// Validate if File can be opened or not
if (!filePointer)
{
perror("fopen");
exit(EXIT_FAILURE);
}
fseek(filePointer,0,SEEK_END);
fileLength=ftell(filePointer); 
rewind(filePointer);
pointerBinaryFile=(signed char *)malloc((fileLength+1)*sizeof(char)); // will return the address to the space 
//printf("\nReading  File from File Location Specified by args...."); 
for(i=0;i<fileLength;i++)
{ 
fread(pointerBinaryFile+i,1,1,filePointer);
} 
fclose(filePointer); 
// Assign this value to the pointer to start it with 1024 location
program_Pointer_VM= (unsigned char *)JVM+IndexForCopy;
green();

//printf("\n File LENGTH: %d SOURCE ADDRESS : %u TARGET ADDRESS : %u\n",fileLength, pointerBinaryFile,program_Pointer_VM);
reset();
instuctionStart=program_Pointer_VM;
//printf("\n ......Copying File to VM at memery location %d\n",IndexForCopy);
memcpy(program_Pointer_VM,pointerBinaryFile,fileLength); // Copied the files to the 1024 location of our VM

int iCount1=0;
red();
// Printing the File 
for(iCount1=0;iCount1<fileLength;iCount1++)
{ 
printf("%hhu\t",*program_Pointer_VM); 
program_Pointer_VM++;
}
printf("\n");

instructionEnd=--program_Pointer_VM; // Adjusting the Last Pointer 
reset();
free(pointerBinaryFile);
return fileLength;
}


// CUSTOM PUSH
 void Push(int item)
 {
    operandStack_Pointer++; 
   *operandStack_Pointer=item;
   
   }
//CUSTOM POP
 int Pop()
 { 
     // Check if StackPointer value is not Less than equal to FramePointer
     if(operandStack_Pointer<=stackFramePointer_Dynamic)
      {
        printf("\n Can not pop.");
      
      }
     else{
         int poppedValue=*operandStack_Pointer;   
          operandStack_Pointer--; 
         return poppedValue;
        }
 }

void Printheader(char *heading)
{


    yellow();
    printf("\n____________________________________________________________________________________________________________________________\n");
    green();
    printf("%s\n", heading);
    reset();
    yellow();
   printf("____________________________________________________________________________________________________________________________\n");
   reset();
}

void Highlight_ProgramByteCode(int indexToHighlight)
{
  int iCount;
  
// Printing the File 
 unsigned char *pointer_to_Program=program_Pointer_VM;
 printf("\n PROGRAM FILE\n");  
for(iCount=0;iCount<execFileLength;iCount++)
{ 
   unsigned char *PointerToReferencedAddress=(unsigned char *) JVM+indexToHighlight;
   if(pointer_to_Program==PointerToReferencedAddress)
   {
         red();
         printf("%hhu\t",*pointer_to_Program); 
         reset();
       
   }
   else
   {
     blue();
     printf("%hhu\t",*pointer_to_Program); 
     reset();
   }
   
     
      pointer_to_Program++;
}
 printf("\n\n");
 reset();
}

void Highlight_ConstantPoolReference(unsigned char * referenceToNewMethod)
{
  int iCount;
  

 unsigned char *pointer_to_Program=constantPool_Pointer_VM;
 printf("\n CONSTANT POOL FILE\n");  
for(iCount=0;iCount<constantPoolFileLength;iCount++)
{ 
   //unsigned char *PointerToReferencedAddress=(unsigned char *) JVM+indexToHighlight;
   if(pointer_to_Program==referenceToNewMethod)
   {
         red();
         printf("%hhu\t",*pointer_to_Program); 
         reset();
       
   }
   else
   {
     blue();
     printf("%hhu\t",*pointer_to_Program); 
     reset();
   }
   
     
      pointer_to_Program++;
}
 printf("\n\n");
 reset();
}

