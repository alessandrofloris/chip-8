#./chip8.o rom/IBM\ Logo.ch8

chip8:
	gcc -o chip8.o chip8.c graphic.c -lSDL2
