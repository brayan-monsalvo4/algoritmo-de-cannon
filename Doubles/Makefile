CC = gcc
BOMPDIR = bin/openmp
BMPIDIR = bin/mpi
UDIR = util
OMPDIR = openmp
SECDIR = bin/secuencial
SECUENDIR = secuencial
CFLAGS= -fopenmp -lm

_DEPS = matrix.h matrix.c
DEPS = $(patsubst %, $(UDIR)/%, $(_DEPS))

all: cannon cannon2 secuencial

cannon: $(OMPDIR)/cannon.c $(DEPS)
	$(CC) -o $(BOMPDIR)/$@ $^ $(CFLAGS)

cannon2: mpi/cannon2.c 
	mpicc $^ -o $(BMPIDIR)/cannon2 -lm


secuencial: $(SECUENDIR)/secuencial.c $(DEPS)
	$(CC) -o $(SECDIR)/$@ $^ $(CFLAGS)


clean:
	rm $(BOMPDIR)/* && rm $(BMPIDIR)/*