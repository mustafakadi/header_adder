CC=gcc
CFLAGS=-I.

headadder: header_adder.c
	$(CC) -Wall -o headadder header_adder.c
