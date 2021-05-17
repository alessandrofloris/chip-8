#./chip8.o rom/test.ch8

chip8:
	gcc -o chip8.o chip8.c graphic.c -lSDL2
