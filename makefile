SRC=graph_basic.c graph_print.c graph_algos.c graphaux.c
OBJ=graph_basic.o graph_print.o graph_algos.o graphaux.o

# version LINUX:
CC = g++
CCFLAGS = -g -DLINUX -Wall

# version HP-UX: 
#CC = cc
#CCFLAGS = -g -DHP -Aa -DDEFTIMEVAL

obj:	$(OBJ)

doc:	graphes.dox $(SRC)
	doxygen graphes.dox; mv doc/html/* Html/doc

clean:
	rm -f $(OBJ); rm -f *.exe

.SUFFIXES:	.exe

.c.exe:	
	$(CC) $(CCFLAGS) $< $(OBJ) -lm -o $@

graphaux.o:	graphes.h graphaux.h graphaux.c
	$(CC) $(CCFLAGS) -c graphaux.c

graph_basic.o:	graphes.h graphaux.h graph_basic.c
	$(CC) $(CCFLAGS) -c graph_basic.c

graph_print.o:	graphes.h graphaux.h graph_print.c
	$(CC) $(CCFLAGS) -c graph_print.c

graph_algos.o:	graphes.h graphaux.h graph_algos.c
	$(CC) $(CCFLAGS) -c graph_algos.c

dijkstra: graphes.h graphaux.o dijkstra.c
	$(CC) $(CCFLAGS) dijkstra.c graphaux.o graphes.h graph_basic.c -o Dijkstra

Aetoile: graphes.h graphaux.o Aetoile.c
	$(CC) $(CCFLAGS) Aetoile.c graphaux.o graphes.h graph_basic.c vdc.c vdc.h -o AEtoile