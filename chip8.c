#include<stdio.h>
#include<stdlib.h>

#include "chip8.h"

#define MEMORY_SIZE 4096 //chip 8 have 4096 memory locations
#define PROGRAM_START_ADDRESS 512 //in the first 512 byte was usually used to store the 
								  //chip-8 interpreter, so the progma will start from the address 512

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
		printf("[Error] Read of the fail failed\n");
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

//decodes the opcode and call the relative function to execute it
void decodeAndExecute(word opcode) {

	switch(opcode) {
		//...
	}
}

//manage the execution of the program in memory
//fetch-decode-execute cycle
void startEmulation() {

	word opcode;

	for(int i=0;i<10;i++) {
		opcode = fetch(); //fetches the opcode from memory

		//decodeAndExecute(opcode); //decodes and executes the opcode
	}

}

int main(int argc, char **argv) {
	
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