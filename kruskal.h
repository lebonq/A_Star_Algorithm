#include "graphaux.h"
#include "graphes.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* ci-dessous trois macros pour pouvoir utiliser les termes, booleens, VRAI et FAUX dans les programmes */
typedef unsigned char booleen;
#define VRAI 1
#define FAUX 0

/* ci-dessous, d�finition d'une structure de liste FIFO d'entiers (pour stocker des ensembles de sommets d'un graphe) */

typedef struct ListeFIFO{
  int n; /* nombre d'elements */
  int prem; /* indice du premier element */
  int der; /* indice du dernier elemeent */
  int capacite; /* nombre d'elements que la liste peut contenir */
  int * elements; /* taleau contenant les elements de la liste*/
} ListeFIFO;

ListeFIFO* initListeFIFO(int capacite);

void termineListeFIFO(ListeFIFO *l);
int selectionSuppressionListeFIFO(ListeFIFO *l);
booleen insertionListeFIFO(ListeFIFO *l, int x);
booleen estNonVideListeFIFO(ListeFIFO *l);
booleen * explorationLargeur(graphe* G, int x);
graphe * fermetureSymEfficace(graphe * g);
booleen * CC(graphe* G, int x);
int Partitionner(int *A, double *T, int p, int r);
int PartitionStochastique(int *A, double *T, int p, int r);
void TriRapideStochastique (int * A, double *T, int p, int r);
int* triAretes(graphe *Gp);
graphe* initGraphMin(graphe* G,double* poidsArbreMin);
int isConnexe(graphe* G, int x);