#include<stdio.h>
#include<stdlib.h>

#define MEMORY_SIZE 4096
#define PROGRAM_START_ADDRESS 512

typedef unsigned char byte;

byte memory[MEMORY_SIZE];

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

int main(int argc, char **argv) {

	//we expect the user to specify the path of the rom to run 
	//when launches the emulator, like "./chip8 path/to/rom"
	//so argc should contain the value 2, and argv[1]
	//should contain the path to the rom 
	if(argc<2) {
		printf("[Error] You have to specify the path to you're ROM, like './chip8 paht/to/ROM'\n");
		exit(EXIT_FAILURE);
	}

	loadRomInMemory(argv[1]); //loads the ROM in memory

}