/*
Graphic

La risoluzione di un display per il chip8 è di 64x32 pixel
La grafica si serve di sprites, di 8 pixel wide e tra 1 e 16 pixel height

I dati riguardanti le sprite dovrebbero essere salvate nell'area 0x000 - 0x1FF
ossia l'area che avrebbe dovuto contenere l'interprete
*/

#include<stdio.h>
#include<stdlib.h>

//reads a binary file from the 
//path specified by the user
//on the startup of the CHIP8 emulator
void loadRomInMemory(char *path) {
	//...
}

int main(int argc, char **argv) {

	//we expect the user to specify the path of the rom to run 
	//when lauches the emulator, like "./chip8 path/to/rom"
	//so argc should contain the value 2, and argv[2]
	//should contain the path to the rom 
	if(argc<2) {
		printf("[Error] You have to specify the path to you're ROM, like './chip8 paht/to/ROM'\n");
		exit(EXIT_FAILURE);
	}

	loadRomInMemory(argv[2]);

	
	
}