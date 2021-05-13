typedef unsigned char byte;
typedef unsigned short word;

//this is the structure which represent our CPU
typedef struct {

	//the PC(program counter) points to the next instruction in memory to fetch 
	word pc;

}CPU;