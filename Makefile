CC=gcc
CFLAGS=-I

build:	main.c ch8_cpu.c ch8_gpu.c util.c
	$(CC) -o bin/ch8 main.c ch8_cpu.c ch8_gpu.c util.c
