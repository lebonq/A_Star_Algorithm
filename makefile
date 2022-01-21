OBJ=graphaux.o

# version LINUX:
CC = g++
CCFLAGS = -g -DLINUX -Wall

all:
	make Aetoile

obj:	$(OBJ)

clean:
	rm -f $(OBJ);

graphaux.o:	graphes.h graphaux.h graphaux.c
	$(CC) $(CCFLAGS) -c graphaux.c

Aetoile: graphes.h graphaux.o
	$(CC) $(CCFLAGS) graphaux.o graphes.h graph_basic.c vdc.c vdc.h kruskal.c kruskal.h -o AEtoile.exe
	make clean