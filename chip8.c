#include<stdio.h>
#include<stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>


#include "chip8.h"
#include "graphic.h"

byte log_ = 1;

byte isRunning = 1;

byte keyboard[N_KEYBOARD_INPUTS]; //keyboard map

byte memory[MEMORY_SIZE]; //our memory

word stack[STACK_SIZE]; //stack

CPU cpu; //our cpu

/**************/
//Graphic
/**************/

byte screen[64][32]; //represent our screen
uint32_t *pixel_buffer; //store the pixel status of the screen

byte draw_screen_flag = 0;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture; //A texture is an image already converted and stored in the graphic card.
 											//Its display is way faster, and also relieves the processor of the machine.

/**************/
//Functions
/**************/

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

	pixel_buffer = malloc((SCREEN_HEIGHT * SCREEN_WIDTH) * sizeof(uint32_t));

	//sets program counter
	//we know that the first instruction is stored
	//in memory at the address 512
	cpu.pc = (word)PROGRAM_START_ADDRESS;

	//setting the stack pointer
	cpu.sp = 0;

	//initialization of the general purpose registers
	for(int i=0;i<N_REGISTERS;i++) {
		cpu.v[i] = 0;
	}

	//clear display
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screen[i][j] = 0;
        }
    }

	//load fontset into memory
	for(int i = 0; i < FONTSET_SIZE; i++) {
		memory[i] = FONTSET[i];
	}

	//...
}

//fetch the next instruction in memory pointed by the pc
word fetch() {

	//since each opcode is 2byte long, and in memory we store
	//1byte per time, we need to use some logic operators to
	//store the full opcode in one variable

	word opcode; //will contain the full opcode
	byte msb = memory[(cpu.pc)++]; //fetch the most significant byte of the opcode
	byte lsb = memory[(cpu.pc)++]; //fetch the least significant byte of the opcode

	opcode = lsb<<8 | msb;

	return opcode;
}

//knowing that in each opcode the "X" is the second
//most significant nibble, like NXNN, we can obtain it
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

void printMemory() {

	for(int i=PROGRAM_START_ADDRESS;i<PROGRAM_START_ADDRESS+100; i++) {
		printf("[%x] %x\n", i, memory[i]);
	}
}

//shows in the console 
//what opcode is being executed and the status of the registers
void debug_(word opcode) {

	printf("Opcode: 0x%x\n", opcode);
	printf("Registers: \n");
	for(int i=0; i<16; i++) {
		printf("\tV[%d] = %x\n", i, cpu.v[i]);
	}
	printf("PC = %x\n", cpu.pc);
	printf("SP = %d\n", cpu.sp);
	printf("I = %d\n", cpu.I);
	printMemory();
	printf("press a key to continue...\n\n");
	getchar();
}

//decodes the opcode and execute it
void decodeAndExecute(word opcode) {

	word x, y, i; //support variables

	if(log_)
		debug_(opcode);

	switch(opcode>>12) {
		case 0x0:
			switch(opcode) {
				case 0x00E0: //clears the screen
					//...
					break;
				case 0x00EE: //returns from a subroutine. The interpreter sets the program counter to the address at the top of the stack,
						     //then subtracts 1 from the stack pointer.
					cpu.pc = stack[cpu.sp--];
					break;
				default: //0NNN, calls machine code routine at address NNN. Not necessary for most ROMs
					//...
					break;
			}
			break;
		case 0x1: //1NNN, jumps to address NNN
			cpu.pc = (opcode&0x0FFF);
			break;
		case 0x2: //2NNN, Calls subroutine at NNN. The interpreter increments the stack pointer, then puts the current PC on the top
				  //of the stack. The PC is then set to nnn
			stack[cpu.sp] = cpu.pc;
			cpu.sp++;
			cpu.pc = (opcode&0x0FFF);
			break;
		case 0x3: //3XNN, Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] == (opcode&0x00FF)) {
				cpu.pc = cpu.pc + 2; //we increment the pc by 2 cause each instruction is 16 byte longs
			}
			break;
		case 0x4: //4XNN, Skips the next instruction if VX does not equal NN. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] != (opcode&0x00FF)) {
				cpu.pc = cpu.pc + 2;
			}
			break;
		case 0x5: //5XY0, Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
			if(cpu.v[getXFromOpcode(opcode)] == cpu.v[getYFromOpcode(opcode)]) {
				cpu.pc = cpu.pc + 2;
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
				cpu.pc = cpu.pc + 2;
			}
			break;
		case 0xA: //ANNN, Sets I to the address NNN
			cpu.I = (opcode&0x0FFF);
			break;
		case 0xB: //BNNN, Jumps to the address NNN plus V0
			cpu.pc = cpu.v[0]+(opcode&0x0FFF);
			break;
		case 0xC: //CXNN, Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
			cpu.v[getXFromOpcode(opcode)] = (rand()%(255+1))&(opcode&0x00FF);
			break;
		case 0xD: //DXYN, Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels.
				  //Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction.
				  //As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen

		    byte sprite_height = (opcode&0x000F)+1;; //sprite height
		    byte x_location = cpu.v[getXFromOpcode(opcode)]; //X coordinate
		    byte y_location = cpu.v[getYFromOpcode(opcode)]; //Y coordinate
		    byte pixel;

		    cpu.v[0xF] = 0; //reset collision register to false (zero)

		    for (int y_coordinate = 0; y_coordinate < sprite_height; y_coordinate++) {
		        pixel = memory[cpu.I + y_coordinate];
		        for (int x_coordinate = 0; x_coordinate < 8; x_coordinate++) {
		            if ((pixel & (0x80 >> x_coordinate)) != 0) {
		                if (screen[y_location + y_coordinate][x_location + x_coordinate] == 1) {
		                    cpu.v[0xF] = 1;
		                }
		                screen[y_location + y_coordinate][x_location + x_coordinate] ^= 1;
		            }
		        }
		    }

		    draw_screen_flag = 1;
		    cpu.pc += 2;

			break;
		case 0xE:
				switch(opcode&0x000F) {
					case 0xE: //EX9E ,Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
						if(keyboard[cpu.v[getXFromOpcode(opcode)]]) { //if the key is pressed
							cpu.pc = cpu.pc + 2;
						}
						break;
					case 0x1: //EXA1, Skips the next instruction if the key stored in VX is not pressed. (Usually the next instruction is a jump to skip a code block)
					if(!keyboard[cpu.v[getXFromOpcode(opcode)]]) { //if the key is not pressed
						cpu.pc = cpu.pc + 2;
					}
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
					cpu.I = cpu.I + cpu.v[getXFromOpcode(opcode)];
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
		default:
				printf("[Warning] Unknown opcode: %x\n", opcode);
		}

		if(log_)
		debug_(opcode);

}

//checks for user inputs
void handleUserInput() {
	SDL_Event e;
    while (SDL_PollEvent(&e)) {

         //checks if the "x" button on the screen window has been clicked
         if (e.type == SDL_QUIT) {
            isRunning = 0;
         }
    }
}

//manage the execution of the program in memory
//fetch-decode-execute cycle
void startEmulation() {

	word opcode;

	//stops when the user click the "x" on the screen window
	while(isRunning){
		opcode = fetch(); //fetches the opcode from memory

		decodeAndExecute(opcode); //decodes and executes the opcode

		handleUserInput(); //handles the user inputs, like key or mouse clicks

		//if the drawScreen flag has been set then updates the screen (and clear the flag)
		if(draw_screen_flag) {
				bufferGraphics(screen, pixel_buffer, renderer);
				drawGraphics(pixel_buffer, renderer, texture);
				draw_screen_flag = 0;
		}

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

	initGraphic(window, renderer, texture); //initialize emulator graphic

	startEmulation(); //starts the execution of the program in memory

	freeGraphic(window, renderer, texture); //close emulator window
}
