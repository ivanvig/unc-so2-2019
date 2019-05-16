CC=gcc
# CFLAGS=-Werror -Wall -pedantic -O3 -march=haswell -mtune=haswell
CFLAGS=-Werror -Wall -pedantic -O3 -march=skylake -mtune=skylake
LIBS = -lnetcdf -fopenmp

all: main

debug: CFLAGS += -g
debug: main

main: conv.c
	$(CC) conv.c -o main $(CFLAGS) $(LIBS)
clean:
	rm main inmatrix.bin outmatrix.bin
