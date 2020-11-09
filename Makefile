CC=cc
CFLAGS=-I

build:	main.c ch8_cpu.c
	$(CC) -o bin/ch8 main.c ch8_cpu.c
