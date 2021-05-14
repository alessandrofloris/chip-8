#include<stdio.h>
#include<stdlib.h>
#include <time.h>

#include "chip8.h"

byte memory[MEMORY_SIZE]; //our memory

CPU cpu; //our cpu

//reads a binary file from the 
//path specified by the user
//on the startup of the CHIP8 emulator
void loadRomInMemory(char *path) {
	
	FILE *ptr = NULL; 
	int address = PROGRAM_START_ADDRESS;
	
	ptr = fopen(path, "rb");

	//error opening the file
	if(ptr == NULL) {
		printf("[Error] Can't open '%s'\n", path);
		exit(EXIT_FAILURE);
	}

	//loading of the file content into memory starting from the 512 address location
	while(fread(&memory[address++], sizeof(byte), 1, ptr) == 1) ;
	
	//check if EOF is reached, or if ferror is set
	if(!feof(ptr) || ferror(ptr)) {
		printf("[Error] Can't read the ROM\n");
		exit(EXIT_FAILURE);
	}

	fclose(ptr);
}

//initialize our emulator
//loads proram in memory
//set registers
//...
void initEmulator(char *path) {

	loadRomInMemory(path); //loads the ROM in memory

	//sets program counter
	//we know that the first instruction is stored 
	//in memory at the address 512
	cpu.pc = (word)PROGRAM_START_ADDRESS;

	//initialization of the general purpose registers
	for(int i=0;i<N_REGISTERS;i++) {
		cpu.v[i] = 0;
	}

	//...
}

//fetch the next instruction in memory pointed by the pc 
word fetch() {

	//since each opcode is 2byte long, and in memory we store 
	//1byte per time, we need to use some logic operators to 
	//store the full opcode in one variable
	word opcode = memory[(cpu.pc)++]; //fetch the first byte of the opcode
	opcode = opcode<<8; //left shift of 8 bit
	opcode = opcode | memory[(cpu.pc)++]; //fetch the second byte of the opcode

	return opcode;
}

//knowing that in each opcode the is is the secondo 
//most significant nibble, like NXNN, we can optain it
byte getXFromOpcode(word opcode) {
	byte x = (byte)opcode>>8;
	x = (x&0x0F);

	return x;
}

//knowing that in each opcode the is is the third 
//most significant nibble, like NNYN, we can optain it
byte getYFromOpcode(word opcode) {
	byte y = (byte)opcode>>4;
	y = (y&0x0F);

	return y;
}

//decodes the opcode and execute it
void decodeAndExecute(word opcode) {

	word x, y; //support variables

	switch(opcode>>12) {
		case 0x0:
			switch(opcode) {
				case 0x00E0: //clears the screen
					//...
					break;
				case 0x00EE: //returns from a subroutine
					//...
					break;
				default: //0NNN, calls machine code routine at address NNN. Not necessary for most ROMs
					//...
					break;
			}
			break;
		case 0x1: //1NNN, jumps to address NNN
			cpu.pc = (opcode&0x0FFF);
			break;
		case 0x2: //2NNN, Calls subroutine at NNN
			//...
			break;	
		case 0x3: //3XNN, Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] == (opcode&0x00FF)) {
				(cpu.pc)++;
			}
			break;
		case 0x4: //4XNN, Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] != (opcode&0x00FF)) {
				(cpu.pc)++;	
			}
			break;
		case 0x5: //5XY0, Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] == cpu.v[getYFromOpcode(opcode)]) {
				(cpu.pc)++;	
			}
			break;
		case 0x6: //6XNN, Sets VX to NN 
			cpu.v[getXFromOpcode(opcode)] = (opcode&0x00FF);
			break;
		case 0x7: //7XNN, Adds NN to VX. (Carry flag is not changed)
			cpu.v[getXFromOpcode(opcode)] += (opcode&0x00FF);
			break;
		case 0x8:
			switch(opcode&0x000F)  {
				case 0x0: //8xy0, Sets VX to the value of VY.
					cpu.v[getXFromOpcode(opcode)] = cpu.v[getYFromOpcode(opcode)]; 
					break;
				case 0x1: //8XY1, Sets VX to VX or VY. (Bitwise OR operation)
					cpu.v[getXFromOpcode(opcode)] |= cpu.v[getYFromOpcode(opcode)]; 
					break;
				case 0x2: //8XY2, Sets VX to VX and VY. (Bitwise AND operation)
					cpu.v[getXFromOpcode(opcode)] &= cpu.v[getYFromOpcode(opcode)]; 
					break;	
				case 0x3: //8XY3, Sets VX to VX xor VY. 
					cpu.v[getXFromOpcode(opcode)] ^= cpu.v[getYFromOpcode(opcode)]; 
					break;
				case 0x4: //8XY4, Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
					x = cpu.v[getXFromOpcode(opcode)]; //i store the content of v[x] and v[y] in a 16bit var
					y = cpu.v[getYFromOpcode(opcode)]; //so i can deduce if there is a carry from the ninth least significant bit 
					x += y;

					if(x>>8) //there's a carry
						cpu.v[0xF] = 1;
					else 
						cpu.v[0xF] = 0;
					
					cpu.v[getXFromOpcode(opcode)] = (byte)y&0x00FF; //only store in v[y] the least significand byte 
					break;
				case 0x5:  //8XY5, VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
					cpu.v[getXFromOpcode(opcode)] = cpu.v[getXFromOpcode(opcode)] - cpu.v[getYFromOpcode(opcode)];
					
					if(cpu.v[getYFromOpcode(opcode)] <= cpu.v[getXFromOpcode(opcode)]) {
						cpu.v[0xF] = 1; //no borrow
					} else {
						cpu.v[0xF] = 0; //borrow
					}  
					break;
				case 0x6:  //8XY6, Stores the least significant bit of VX in VF and then shifts VX to the right by 1
					cpu.v[0xF] = (cpu.v[getXFromOpcode(opcode)]&0x0001);
					cpu.v[getXFromOpcode(opcode)] = (cpu.v[getXFromOpcode(opcode)]>>1);
					break;
				case 0x7:  //8XY7, Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not
					cpu.v[getXFromOpcode(opcode)] = cpu.v[getYFromOpcode(opcode)] - cpu.v[getXFromOpcode(opcode)];
					
					if(cpu.v[getXFromOpcode(opcode)] <= cpu.v[getYFromOpcode(opcode)]) {
						cpu.v[0xF] = 1; //no borrow
					} else {
						cpu.v[0xF] = 0; //borrow
					}  
					break;
				case 0xE: //8XYE, Stores the most significant bit of VX in VF and then shifts VX to the left by 1
					cpu.v[0xF] = ((cpu.v[getXFromOpcode(opcode)]>>7)&0x0001);
					cpu.v[getXFromOpcode(opcode)] = (cpu.v[getXFromOpcode(opcode)]<<1);
					break;		
			}
			break;	
		case 0x9: //9XY0, Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] != cpu.v[getYFromOpcode(opcode)]) {
				(cpu.pc)++;
			}
			break;
		case 0xA: //ANNN, Sets I to the address NNN
			cpu.I = (opcode&0x0FFF);
			break;	
		case 0xB: //BNNN, Jumps to the address NNN plus V0
			cpu.pc = cpu.v[0x0]+(opcode&0x0FFF);
			break;	
		case 0xC: //CXNN, Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
			cpu.v[getXFromOpcode(opcode)] = (rand()%(255+1))&(opcode&0x00FF);
			break;
		case 0xD: //DXYN, Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels.
				  //Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction.
				  //As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen
			//...
			break;
		case 0xE: 
				switch(opcode&0x000F) {
					case 0xE: //EX9E ,Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
						//...
						break;
					case 0x1: //EXA1, Skips the next instruction if the key stored in VX is not pressed. (Usually the next instruction is a jump to skip a code block)
						//...
						break;
				}	
			break;
		case 0xF: 
			switch(opcode&0x00FF) {
				case 0x07: //FX07, Sets VX to the value of the delay timer.
					//...
					break;
				case 0x0A: //FX0A, A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
					//...
					break;	
				case 0x15: //FX15, Sets the delay timer to VX.
					//...
					break;
				case 0x18: //FX18, Sets the sound timer to VX.
					//...
					break;
				case 0x1E: //FX1E, Adds VX to I. VF is not affected
					//...
					break;
				case 0x29: //FX29, Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
					//...
					break;
				case 0x33: //FX33, Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, 
						   //the middle digit at I plus 1, and the least significant digit at I plus 2. 
						   //(In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, 
						   //the tens digit at location I+1, and the ones digit at location I+2.)
					//...
					break;
				case 0x55: //FX55, Stores V0 to VX (including VX) in memory starting at address I. 
						   //The offset from I is increased by 1 for each value written, but I itself is left unmodified
					//...
					break;
				case 0x65: //FX65, Fills V0 to VX (including VX) with values from memory starting at address I. 
						   //The offset from I is increased by 1 for each value written, but I itself is left unmodified.
					//...
					break;	   		   		   				
			}
			break;
		}
}

//manage the execution of the program in memory
//fetch-decode-execute cycle
void startEmulation() {

	word opcode;

	for(int i=0;i<100;i++) {
		opcode = fetch(); //fetches the opcode from memory

		decodeAndExecute(opcode); //decodes and executes the opcode
	}

}

int main(int argc, char **argv) {
	
	srand(time(NULL));
	//we expect the user to specify the path of the rom to run 
	//when launches the emulator, like "./chip8 path/to/rom"
	//so argc should contain the value 2, and argv[1]
	//should contain the path to the rom 
	if(argc<2) {
		printf("[Error] You have to specify the path to you're ROM, like './chip8 paht/to/ROM'\n");
		exit(EXIT_FAILURE);
	}

	initEmulator(argv[1]); //initialize our emulator

	startEmulation(); //starts the execution of the program in memory
}