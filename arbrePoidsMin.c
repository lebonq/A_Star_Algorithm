#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#include"graphes.h"

/* ci-dessous trois macros pour pouvoir utiliser les termes, booleens, VRAI et FAUX dans les programmes */
typedef unsigned char booleen;
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
booleen * explorationLargeur(graphe* G, int x){
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
  
  for(k = 1 ; k < G->nsom; k++){

    while(estNonVideListeFIFO(E)){
      y = selectionSuppressionListeFIFO(E);
      for(p = G->gamma[y]; p != NULL; p = p->suivant){
	z = p->som;
	if(Z[z] == FAUX){
	  insertionListeFIFO(D, z);
	  Z[z] = VRAI;
	}
      }
    }
    /* On echange E et D */
    tmp = E;
    E = D;
    D = tmp;
      
    }
  termineListeFIFO(D);
  termineListeFIFO(E);
  
  return Z;
}

/* ====================================================================== */
/*! \fn graphe * fermetureSymEfficace(graphe * g)
    \param g (entrée) : un graphe.
    \return un graphe.
    \brief construit et retourne la fermeture symétrique du graphe g. 
    \warning  L'algorithme est efficace (complexité lineaire) :-).
*/
graphe * fermetureSymEfficace(graphe * g)
/* ====================================================================== */
{
  graphe *g_1;
  int nsom, narc, k, x, y;
  pcell p;

  nsom = g->nsom;
  narc = g->narc;
  g_1 = initGraphe(nsom, 2*narc);

  for (x = 0; x < nsom; x++) /* pour tout i sommet de g */
    for(p = g->gamma[x]; p != NULL; p = p->suivant){
      ajouteSuccesseur(g_1, p->som, x);
      ajouteSuccesseur(g_1, x, p->som);
    }
  return g_1;
} /* Sym() */

/***************************************************************************/
/* retourne la composante connexe du graphe G contenant le sommet x        */
/* en utilisant l'algorithme exploration largeur (version 1)               */
/***************************************************************************/
booleen * CC(graphe* G, int x){
  booleen *Z;
  graphe *GG = fermetureSymEfficace(G);
  Z = explorationLargeur(GG, x);
  termineGraphe(GG);
  return Z;
}



/*************************************************
  Tri rapide et selection
  D'apres "Introduction a l'algorithmique", 
    T. Cormen, C. Leiserson, R. Rivest, pp. 152, Dunod Ed. 

  Michel Couprie  -  Decembre 1997

  Version avec index et cle - Septembre 1999
  Les donnees cle sont contenues dans un tableau T.
  Le tri s'effectue sur un tableau A contenant les index
    des elements de T.
**************************************************/

/* =============================================================== */
int Partitionner(int *A, double *T, int p, int r)
/* =============================================================== */
/*
  partitionne les elements de A entre l'indice p (compris) et l'indice r (compris)
  en deux groupes : les elements q tq T[A[q]] <= T[A[p]] et les autres.
*/
{
  int t;
  double x = T[A[p]];
  int i = p - 1;
  int j = r + 1;
  while (1)
  {
    do j--; while (T[A[j]] > x);
    do i++; while (T[A[i]] < x);
    if (i < j) { t = A[i]; A[i] = A[j]; A[j] = t; }
    else return j;
  } /* while (1) */   
} /* Partitionner() */

/* =============================================================== */
int PartitionStochastique(int *A, double *T, int p, int r)
/* =============================================================== */
/*
  partitionne les elements de A entre l'indice p (compris) et l'indice r (compris)
  en deux groupes : les elements k tels que T[A[k]] <= T[A[q]] et les autres, 
  avec q tire au hasard dans [p,r].
*/
{
  int t, q;

  q = p + (rand() % (r - p + 1));
  t = A[p];         /* echange A[p] et A[q] */
  A[p] = A[q]; 
  A[q] = t;
  return Partitionner(A, T, p, r);
} /* PartitionStochastique() */

/* =============================================================== */
/*! \fn void TriRapideStochastique (int * A, double *T, int p, int r)
    \param A (entrée/sortie) : un tableau d'entiers
    \param T (entrée) : un tableau de valeurs de type double.
    \param p (entrée) : indice du début de la zone à trier.
    \param r (entrée) : indice de fin de la zone à trier.
    \brief tri par ordre croissant des valeurs du tableau \b T.
           Le tri s'effectue sur un tableau \b A contenant les index
           des elements de \b T, l'indice \b p (compris) à l'indice \b r (compris).
*/
void TriRapideStochastique (int * A, double *T, int p, int r)
/* =============================================================== */
{
  int q; 
  if (p < r)
  {
    q = PartitionStochastique(A, T, p, r);
    TriRapideStochastique (A, T, p, q) ;
    TriRapideStochastique (A, T, q+1, r) ;
  }
} /* TriRapideStochastique() */


int* triAretes(graphePondere *Gp){
  int *O;
  int i;
  
  O = (int *)malloc(Gp->narc * sizeof(int)); /* allocation index */
  if (O == NULL)
  {   fprintf(stderr, "triArete : malloc failed\n");
      exit(0);
  }  
  for (i = 0; i < Gp->narc; i++) O[i] = i; /* indexation initiale */
  TriRapideStochastique(O, Gp->poids, 0, Gp->narc-1);
  return O;
}

int * arborescence(graphe* G, int x){
  booleen *Z;       /* tableau booleens pour stocker l'exploration */
  ListeFIFO *E, *D; /* Liste pour les ensembles de sommets */
  ListeFIFO *tmp;   /* variable temporaire pour permettre l'echange des liste E et D */
  pcell p; /* pointeur-maillon pour parcourir des listes de successeurs */
  int y, z;         /* sommets du graphe */
  int k;            /* numéro d'iteration */

  int * Pred = (int*) calloc(G->nsom, sizeof(int));
  Pred[x] = -1;
  /* initialisation de l'ensemble E : E := {x}*/
  E = initListeFIFO(G->nsom);
  insertionListeFIFO(E, x);
  /* initialisation de l'ensemble Z : Z := {x}*/
  Z = (booleen*) calloc(G->nsom, sizeof(booleen));
  Z[x] = VRAI;
  /* initialisation de l'ensemble D */
  D = initListeFIFO(G->nsom);
  
  for(k = 1 ; k < G->nsom; k++){
    while(estNonVideListeFIFO(E)){
      y = selectionSuppressionListeFIFO(E);
      for(p = G->gamma[y]; p != NULL; p = p->suivant){
	z = p->som;
	if(Z[z] == FAUX){
	  insertionListeFIFO(D, z);
	  Z[z] = VRAI;
	  Pred[z] = y;
	}
      }
    }
    /* On echange E et D */
    tmp = E;
    E = D;
    D = tmp;
      
    }
  termineListeFIFO(D);
  termineListeFIFO(E);
  
  return Pred;
}

graphe * algoKruskal(graphePondere *Gp){
  graphe *T; /* pour stocker l'arbre de poids minimum */
  int *O;          /* tableau pour ranger les index des arcs par ordre croissant de poids*/
  int x, y; // index pour des sommets
  booleen *Z; // pour stocker les composantes connexes
  double poidsArbre = 0; // pour calculer le poids de l'arbre de poids min
  int i=0, k=0;

  T = initGraphe(Gp->nsom, Gp->nsom-1);

  O = triAretes(Gp); /* O[i] est l'index de la i-ème arête par ordre croissant de poids; les extremites de la i-ème aretes sont donc Gp->I[O[i]] et Gp->T[O[i]] */

  /* A completer ici */
  while(k<((Gp->nsom)-1)){
    x = Gp->I[O[i]];
    y = Gp->T[O[i]];
    Z = CC(T,x);
    if(!Z[y]){
      ajouteSuccesseur(T, x, y);
      k++;
    }
    i++;
  }
  //printf("Le poids de l'arbre de poids minimum vaut : %lg \n", poidsArbre);
  return T;
  
}


/* ====================================================================== */
int main(int argc, char **argv)
/* ====================================================================== */
{
  graphePondere *Gp;
  graphe *T;
  int na, ns;     /* nombre d'arcs, nombre de sommets */
  clock_t debut, fin;
  
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <donnee.graphe> <resultat.graphe>\n", argv[0]);
    exit(0);
  }

  Gp = litGraphePondere(argv[1]);       /* Lit un graphe pondere stockée dans un fichier */

  fprintf(stderr, "%s lu\n", argv[1]);
  
  debut =  clock();

  T = algoKruskal(Gp);       /* traitement : calcule le symetrique de g */
  
  fin = clock();

  fprintf(stderr, "APmin calcule en %lf secondes\n", ((double)fin-debut)/CLOCKS_PER_SEC);
  
  graphe * fermT = fermetureSymEfficace(T);
  int * Pred = arborescence(fermT,7);
  for(int i=0 ; i<fermT->nsom ; i++){
    if(Pred[i]!=-1){
      printf("%d Recois le message de %d\n",i,Pred[i]);
    }
  }
   
  /* observer que l'on n'a pas recopié l'arbre de poids minimum les informations relative au sommet, on peut le faire avec la fonction cpyInfo2(Gp,T) */

  cpyInfo2(Gp,T);  
  sauvegardeGraphe(T, argv[2], 11); /* Sauvegarde le résultat dans un fichier */
 
  
  termineGraphePondere(Gp);
  termineGraphe(T);

  return 0;
} /* main() */


