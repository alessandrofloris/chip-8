#define N_KEYBOARD_INPUTS 16 //chip 8 has 16-key keyboard

#define N_REGISTERS 16 //chip 8 has 16 general purpose registers

#define MEMORY_SIZE 4096 //chip 8 has 4096 memory locations

#define STACK_SIZE 16 //chip 8 has 16 levels of stack

#define PROGRAM_START_ADDRESS 512 //in the first 512 byte was usually used to store the
								  //chip-8 interpreter, so the progma will start from the address 512

#define FONTSET_SIZE 80 //the fontset size

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

	//8bit stack pointer, points to the top of the stack
	byte sp;

}CPU;

//this font set can be used to display any hexadecimal number
const static byte FONTSET[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
