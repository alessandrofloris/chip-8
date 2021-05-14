#define N_REGISTERS 16 //chip 8 has 16 general purpose registers

#define MEMORY_SIZE 4096 //chip 8 have 4096 memory locations

#define PROGRAM_START_ADDRESS 512 //in the first 512 byte was usually used to store the 
								  //chip-8 interpreter, so the progma will start from the address 512

typedef unsigned char byte;
typedef unsigned short word;

//this is the structure which represent our CPU
typedef struct {

	//the PC(program counter) points to the next instruction in memory to fetch 
	word pc;

	//our cpu has 16 8bit general purpose register, from V0 to VF
	byte v[N_REGISTERS]; 

	//the 16bit address register
	word I;

}CPU;