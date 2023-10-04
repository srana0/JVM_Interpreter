# JAVA VIRTUAL MACHINE

Your code is expected to be compiled and executed on a linux machine with a gcc compiler.
The standard configuration in our labs. Please ensure this is the case before you submit as the TA needs to be able to run your code and cannot do so if it is not in this environment.

For this assignment we are going to extend the interpreter that you built in assignment #1.Specifically, we are going to add support for object definitions, allocation and
manipulation. This will result in the implementation of a heap for your virtual machine. 

For now, you can assume that the heap will start at address (232 – 1) and grow down in memory addresses. Also, the heap is very forgiving in that we do not deallocate objects
from the heap J For simplicity we will also assume that the heap and the stack do not grow to collide in the middle.

Before we allocate objects, we need to first have the definition of an object. The definition of an object includes the fields of an object as well as the methods of an object.
For simplicity, our objects are going to just contain fields and no methods. The definition of an object is as follows:
|size (unsigned 8-bit number)|Bit encoding field types (24-bits)|
|--|--|


The size is an unsigned int ranging from 1 to 24. Note that it does not make sense to have an object of size 0 (no fields) since there are already no methods. The bit encoding tells whether the field is of type int (0) or reference (1). So, “0000001 |100000000000000000000000” would be an object containing one field which is of type
reference.

The object definitions are given as part of the constant pool to be loaded. As before, method definitions will start at address 256. This will be padded in the constant pool file such that object definitions will start at address 768. So, nothing changes with loading
your constant pool – just the constant pool file will be changed so that object definitions appear at address 768.
To work with objects we need to add support for additional instructions. Below is the extended instruction set that you must support.

Expanded Instruction Set
* new
* newarray (note only int and reference types are supported)
* anewarray
* getfield
* putfield
* aload
* astore
* dup
* aconst_null
* aastore
* aaload

While not expected, students are free to submit their own developed tests with their submissions. 
