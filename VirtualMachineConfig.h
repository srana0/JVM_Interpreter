
// Method Defination for Push and Pop
  int Pop();
  void Push(int item);
  
void Printheader(char *heading);


void BIPUSH();
void ISTORE();
void ICONST_0();
void ILOAD();
void IFGT();
void GOTO();
void ISUB();
void IADD();
void ICMPEQ();
void IINC();
void IWRITE();
void IALOAD();
void IAND();
void IASTORE();
void IDIV();
void IFEQ();
void IFLT();
void IFGE();
void IFLE();
void IFNULL();
void IMULL();
void INEG(); 
void IOR();
void POP_OperandStack();
void IRETURN();
void INVOKE_STATIC();
void IREAD();
void RETURN();
void NEW();
void ANEWARRAY(); 
void NEWARRAY();
void GETFIELD();
void PUTFIELD();
void ALOAD();
void ASTORE();
void DUP();
void ACONST_NULL();
void AASTORE();
void AALOAD();


//void ChangePointersOfStackFrame(uint32_t methodReference,int *argumentPointer);

//Validate whether the input parameters are correct, It also checks number of arguments passed into Main Method
//bool ValidateInputParameter(char *argv[]);
void PrintStackFrame();
int LoadBinaryFileIntoVM(char *argv[],int argmentPostion,int IndexForCopy,int * JVM);
void SaveValueInStackFrame(uint32_t stackFrame_ReturnValue, 
                                        uint32_t programCounter_Address,                      
                                        uint32_t operandStack_Address,
                                        uint32_t stackFrame_Address,
                                        uint32_t numberOfArguments,
                                         int *parameterPointer);

//bool ValidateInputParameter(char *argv[]);
//void Highlight_ObjectReference(unsigned char * referenceToNewObject);
void Highlight_ConstantPoolReference(unsigned char * referenceToNewMethod);
void Highlight_ProgramByteCode(int indexToHighlight);




void PrintVirtualMachine(int * JVM,unsigned char * program_Pointer_VM,int execFileLength,unsigned char * constantPool_Pointer_VM,int constantPoolFileLength,int * stackFramePointer_Static);

// Definations of the Functions
//Below functions will print color in console
void red ();
void green ();
void blue ();
void reset ();
void yellow();

int* ConvertAddress_From_LocalMachine(uint32_t input);
uint32_t ConvertAddress_To_LocalMachine(void* address);

//The Opcodes for JVM Instruction
// typedef enum {
//     HALT=0xb1,  
//     BIPUSH = 0X10,
//     GOTO  = 0Xa7,
//     IADD  =0X60,  
//     IALOAD=0X2e, 
//     I=0X7e,
//     iastore   = 0X4f,
//     iconst_0  = 0X03,
//     iconst_1=0x04,//extra    
//     idiv  = 0X6c, 
//     iinc = 0X84,
//     if_icmpeq= 0X9f,
//     ifeq  = 0X99,/*  */
//     iflt   = 0X9b,
//     ifgt  = 0X9d,
//     ifge  = 0X9c,
//     ifle = 0X9e,
//     ifnull = 0Xc6,
//     iload=0X15,
//     iload_0 =0X1a,//extra
//     iload_1 =0x1b,//extra
//     iload_2 =0X1c,//extra
//     iload_3  =0X1d,///extra
//     istore=0X36,
//     istore_0=0X3b,//EXTRA
//     istore_1=0X3c,//EXTRA
//     istore_2=0X3d,//EXTRA
//     istore_3=0X3e,//ETRA
//     imul=0X68,      
//     ineg=0X74,
//     ior=0X80,
//     ISUB=0X64,
//     IINVOKESTATIC=0Xb8,
//     POP=0X57,
//     IREAD=0xfe,
//     IWRITE=0Xff,
//     IRETURN=0Xac,
//     RETURN= 0xb1
// } VirtualMachine_Opcode;



    // ALWAYS SAVE THE VALUE BY TRIMMING IT 

          // Get the address of the Next program Counter
         // unsigned char * temp_previous_PC=interpreter_ProgramCounter+3;


//INVOKE STATIC
// // This will change the pointers
// void ChangePointersOfStackFrame(uint32_t methodReference,int *argumentPointer)
// {

//   interpreter_ProgramCounter= (signed char *)JVM+methodReference;         // Reassignment : Program Counter
//   stackFramePointer_Static=operandStack_Pointer+1;                        // Reassignment : Static Stack Frame pointer
//   stackFramePointer_Dynamic=stackFramePointer_Static+19;                  // Reassignment : Dynamic Stack Frame pointer
//   operandStack_Pointer=stackFramePointer_Dynamic+1;                       // Reassignment : Operand Stack pointer
//   stackFrame_localVariableIndexCounter=(stackFramePointer_Static+4);      // Reassignment : localVariableIndexCounter pointer

// }

// printf("\n BEFORE CONVERTING ");
// printf("\n interpreter_ProgramCounter+3    %ld (%p) value= %u",(interpreter_ProgramCounter+3),(interpreter_ProgramCounter+3),*(interpreter_ProgramCounter+3));
// printf("\n operandStack_Pointer            %ld (%p) value= %u",operandStack_Pointer,operandStack_Pointer,*operandStack_Pointer);
// printf("\n stackFramePointer_Static        %ld (%p) value= %u",stackFramePointer_Static,stackFramePointer_Static,*stackFramePointer_Static);

// printf("\n  AFTER LOGICAL ADDREES");
// printf("\n (uint32_t)(Convert_To_LogicalAddres(interpreter_ProgramCounter+3)=  %u",((uint32_t)(Convert_To_LogicalAddres(interpreter_ProgramCounter+3))));
// printf("\n (uint32_t)(Convert_To_LogicalAddres(operandStack_Pointer))=         %u",((uint32_t)(Convert_To_LogicalAddres(operandStack_Pointer))));
// printf("\n (uint32_t)(Convert_To_LogicalAddres(stackFramePointer_Static))=     %u",((uint32_t)(Convert_To_LogicalAddres(stackFramePointer_Static))));

//*********************************************
// convert to Logical address

// From this logical address, we need to get the physical address
//unsigned char *next_PC_Pointer=(unsigned char *)(Convert_To_PhysicalAddress(((uint32_t)(Convert_To_LogicalAddres(interpreter_ProgramCounter+3)))));
//printf("\n next PC address %ld (%p) value %u",next_PC_Pointer,next_PC_Pointer,*next_PC_Pointer);

////uint32_t logical_PC_Address=(uint32_t)(Convert_To_LogicalAddres(interpreter_ProgramCounter+3));
//printf("\n logical_PC_Address  value %u",logical_PC_Address);

//int *pointer_to_Physical_address=(Convert_To_PhysicalAddress(logical_PC_Address));
//printf("\n pointer_to_Physical_address address %ld (%p) value=%u",pointer_to_Physical_address,pointer_to_Physical_address,*pointer_to_Physical_address);

//printf("\n again to physical address\n\n");
//printf("\n Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(temp_previous_PC)))=         %ld value =%u",Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(temp_previous_PC))),*temp_previous_PC);
//printf("\n Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(operandStack_Pointer))) =     %ld value= %u",Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(operandStack_Pointer))),*(Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(operandStack_Pointer)))));
//printf("\n Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(stackFramePointer_Static))) = %ld value= %u",Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(stackFramePointer_Static))),*(Convert_To_PhysicalAddress((uint32_t)(Convert_To_LogicalAddres(stackFramePointer_Static)))));