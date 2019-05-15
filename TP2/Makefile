CC=gcc
# CFLAGS=-Werror -Wall -pedantic -O3 -march=haswell -mtune=haswell
CFLAGS=-Werror -Wall -pedantic -march=haswell -mtune=haswell
# NCFLAGS = -lnetcdf -lz -lm -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5_hl -lhdf5
NCLIB = -lnetcdf

all: main

debug: CFLAGS += -g
debug: main

main: conv.c
	$(CC) conv.c -o main $(CFLAGS) $(NCLIB)
clean:
	rm main inmatrix.bin outmatrix.bin
