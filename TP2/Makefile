CC=gcc
# CC=icc -xSKYLAKE-AVX512
# CC=icc
NCPATH =$(HOME)/Development/SO2/hpc/libs/netcdf
LIBS =-lnetcdf -fopenmp
CFLAGS=-Werror -Wall -pedantic -O3 -march=core-avx2 -mtune=core-avx2 -std=c99 -Wl,-rpath,$(NCPATH)/lib

# CFLAGS=-Werror -Wall -pedantic -O3 -march=skylake-avx512 -mtune=skylake-avx512 -std=c99 -Wl,-rpath,$(NCPATH)/lib
# CFLAGS=-Werror -Wall -pedantic -O3 -mtune=skylake-avx512 -std=c99 -Wl,-rpath,$(NCPATH)/lib
# CFLAGS=-Werror -Wall -pedantic -O3 -std=c99 -Wl,-rpath,$(NCPATH)/lib

all: main

debug: CFLAGS += -g
debug: main

main: conv.c
	$(CC) conv.c -o main $(CFLAGS) -I$(NCPATH)/include -L$(NCPATH)/lib $(LIBS)
clean:
	rm main inmatrix.bin outmatrix.bin
