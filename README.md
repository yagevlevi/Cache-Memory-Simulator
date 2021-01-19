# Cache Memory Simulator

Task: Write a cache simulator in csim.c that takes a valgrind memory trace as input, simulates the hit/miss behavior of a cache memory on this trace, and outputs the total number of hits, misses, and evictions. The simulator must work correctly for arbitrary s, E, and b. This means that storage will need to be allocated for the simulator’s data structures using the malloc function. It should be assumed that memory accesses are perfectly aligned.

Valgrind memory traces have the following form:

I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
 
Each line denotes one or two memory accesses. The format of each line is

[space]operation address,size

The operation field denotes the type of memory access: “I” denotes an instruction load, “L” a data load, “S” a data store, and “M” a data modify (i.e., a data load followed by a data store). There is never a space before each “I”. There is always a space before each “M”, “L”, and “S”. The address field specifies a 64-bit hexadecimal memory address. The size field specifies the number of bytes accessed by the operation.

