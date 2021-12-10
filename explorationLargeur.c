#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "graphes.h"

/* ci-dessous trois macros pour pouvoir utiliser les termes, booleens, VRAI et FAUX dans les programmes */
#define booleen unsigned char
#define VRAI 1
#define FAUX 0


/* ci-dessous, définition d'une structure de liste FIFO d'entiers (pour stocker des ensembles de sommets d'un graphe) */

typedef struct {
  int n; /* nombre d'elements */
  int prem; /* indice du premier element */
  int der; /* indice du dernier elemeent */
  int capacite; /* nombre d'elements que la liste peut contenir */
  int * elements; /* taleau contenant les elements de la liste*/
} ListeFIFO;

ListeFIFO* initListeFIFO(int capacite){
  ListeFIFO* l;
  l = (ListeFIFO*)malloc(sizeof(ListeFIFO));
  l->n = 0;
  l->capacite = capacite;
  l->prem = 0;
  l->der = 0;
  l->elements = (int*)calloc(capacite, sizeof(int));
  return l;
}

void termineListeFIFO(ListeFIFO *l){
  free(l->elements);
  free(l);  
}

int selectionSuppressionListeFIFO(ListeFIFO *l){
  int el;
  if (l->n > 0){
    el = l->elements[l->prem];
    l->n --;
    l->prem = (l->prem+1)%l->capacite;
    return el;
  }
  else{
    fprintf(stderr, "Erreur : liste vide\n");
    return -1;
  }      
}

booleen insertionListeFIFO(ListeFIFO *l, int x){
  if (l->n < l->capacite){
    l->elements[l->der] = x;
    l->der = (l->der+1)%l->capacite;
    l->n ++;
    return VRAI; /* pour indiquer que l'insertion ns'est bien deroulee */
  }
  else{
    fprintf(stderr, "Erreur : liste pleine \n");
    return FAUX; /* pour indiquer que l'insertion n'a pas etre effectuee */
  }      
}


booleen estNonVideListeFIFO(ListeFIFO *l){
  if(l->n == 0) return FAUX;
  else return VRAI;
}

/***************************************************************************/
/* retourne l'exploration du graphe G depuis le sommet x                   */
/* en utilisant l'algorithme exploration largeur (version 1)               */
/***************************************************************************/
booleen * arborescence(graphe* G, int x){
  booleen *Z;       /* tableau booleens pour stocker l'exploration */
  ListeFIFO *E, *D; /* Liste pour les ensembles de sommets */
  ListeFIFO *tmp;   /* variable temporaire pour permettre l'echange des liste E et D */
  pcell p; /* pointeur-maillon pour parcourir des listes de successeurs */
  int y, z;         /* sommets du graphe */
  int k;            /* numéro d'iteration */

  
  /* initialisation de l'ensemble E : E := {x}*/
  E = initListeFIFO(G->nsom);
  insertionListeFIFO(E, x);
  /* initialisation de l'ensemble Z : Z := {x}*/
  Z = (booleen*) calloc(G->nsom, sizeof(booleen));
  Z[x] = VRAI;
  /* initialisation de l'ensemble D */
  D = initListeFIFO(G->nsom);

  /* Completer ici avec le code l'algorithme exploration largeur */
  while(estNonVideListeFIFO(E)){
    y = selectionSuppressionListeFIFO(E);
    for(p = G->gamma[y]; p != NULL; p = p->suivant){
      if(!Z[p->som]){
        insertionListeFIFO(E, p->som);
        Z[p->som] = VRAI;
      }
    }
  }
  
  termineListeFIFO(D);
  termineListeFIFO(E);
  
  return Z;
}

/* ====================================================================== */
int main(int argc, char **argv)
/* ====================================================================== */
{
  graphe *G;
  int x;               /* sommet de depart à explorer */
  int y;               /* indice muet pour un sommet*/
  booleen *Z;          /* tableau pour stocker l'exploration du graphe */
  int i;               /* indice muet*/
  int nZ;              /* pour compter le nombre de sommets explorés */
  clock_t debut, fin;  /* pour mesurer les temps d'execution */
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <donnee.graphe> <sommet_initial_a_explorer> \n", argv[0]);
    fprintf(stderr, "            ou %s explore le graphe donnee.graphe deuis sommet_initial_a_explorer\n", argv[0]);
    exit(0);
  }

  /**********************************************************/
  /* Lecture des arguments passes au programmme             */
  /**********************************************************/
  
  G = litGraphe(argv[1]);       /* Lit un graphe stockée dans un fichier */
  
  x = atoi(argv[2]);

  if(x >= G->nsom){
    fprintf(stderr, "Erreur: pas de sommet d'indice %d dans %s\n", x, argv[1]);
    exit(0);
  }

  fprintf(stderr, "%s lu\n", argv[1]);
  
  debut =  clock();

  Z = explorationLargeur(G,x);       /* traitement : calcule le symetrique de g */

  fin = clock();

  /* On afffiche l'exploration et on compte le nombre de sommets explorés */
  nZ = 0;
  printf("L'exploration depuis le sommet %d contient les sommets", x);
  for(y = 0; y < G->nsom; y++)
    if(Z[y] == VRAI){
      printf(" %d", y);
      nZ++;
    }
  printf(".\n");

  /* si un nom est diponible pour les sommets, on affiche aussi les noms */
  if( (G->info != NULL) && G->info[0].nom != NULL){
    printf("--\nDont les noms sont :\n");
    for(y = 0; y < G->nsom; y++)
      if(Z[y] == VRAI)
	printf("%s\n", G->info[y].nom);
  }

  printf("%d sommets poeuvent etre atteints depuis %d et %d sommets ne peuvent pas etre atteints \n", nZ, x, G->nsom - nZ);
  
  free(Z);

  printf("Exploration calculee en %lf secondes\n", ((double)fin-debut)/CLOCKS_PER_SEC);

  return 0;
} /* main() */

