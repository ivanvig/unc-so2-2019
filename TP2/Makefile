CC=gcc
CFLAGS=-Werror -Wall -pedantic
NCFLAGS = -lnetcdf -lhdf5_hl -lhdf5 -lz -lm

all: main

debug: CFLAGS += -g
debug: main

main: conv.c
	$(CC) $(CFLAGS) $(NCFLAGS) conv.c -o main
clean:
	rm main inmatrix.bin outmatrix.bin
