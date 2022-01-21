#ifdef GRAPHE_INC
#include "graphaux.h"
#include "graphes.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#endif
#define GRAPHE_INC

#include "kruskal.h"

/* ====================================================================== */
/*! \fn ListeFIFO* initListeFIFO(int capacite)
    \param capacite : capacité de la liste
    \return pointeur sur la liste alloué
    \brief initialise une liste FIFO
*/
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

/* ====================================================================== */
/*! \fn void termineListeFIFO(ListeFIFO *l)
    \param l : liste FIFO
    \brief free la liste FIFO et ses éléments
*/
void termineListeFIFO(ListeFIFO *l){
  free(l->elements);
  free(l);  
}

/* ====================================================================== */
/*! \fn int selectionSuppressionListeFIFO(ListeFIFO *l)
    \param l : liste FIFO
    \return élément retiré de la liste, ou -1
    \brief retire un élément de la liste
*/
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

/* ====================================================================== */
/*! \fn booleen insertionListeFIFO(ListeFIFO *l, int x)
    \param l : liste FIFO
    \param x : élément de type int
    \return 1 ou 0, selon si l'insertion s'est effectuée ou non
    \brief ajoute un élément à la liste FIFO
*/
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

/* ====================================================================== */
/*! \fn booleen estNonVideListeFIFO(ListeFIFO *l)
    \param l : liste FIFO
    \return 0 si la liste est vide, 1 sinon
    \brief dit si la liste FIFO est vide ou non
*/
booleen estNonVideListeFIFO(ListeFIFO *l){
  if(l->n == 0) return FAUX;
  else return VRAI;
}

/* ====================================================================== */
/*! \fn booleen * explorationLargeur(graphe* G, int x)
    \param G : graphe
    \param x : un sommet du graphe
    \return un tableau de booléens
    \brief retourne l'exploration en largeur à partir du sommet x sur le graphe G
*/
booleen * explorationLargeur(graphe* G, int x){
  booleen *Z;       /* tableau booleens pour stocker l'exploration */
  ListeFIFO *E, *D; /* Liste pour les ensembles de sommets */
  ListeFIFO *tmp;   /* variable temporaire pour permettre l'echange des liste E et D */
  pcell p; /* pointeur-maillon pour parcourir des listes de successeurs */
  int y, z;         /* sommets du graphe */
  int k;            /* num�ro d'iteration */

  
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
      for(p = G->gamma[y]; p != NULL; p = p->next){
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
    \param g (entr�e) : un graphe.
    \return un graphe.
    \brief construit et retourne la fermeture sym�trique du graphe g. 
    \warning  L'algorithme est efficace (complexit� lineaire) :-).
*/
graphe * fermetureSymEfficace(graphe * g)
/* ====================================================================== */
{
  graphe *g_1;
  int nsom, narc, x;
  pcell p;
  int i = 0;
  int j = 0;

  nsom = g->nsom;
  narc = g->narc;

  if(narc < 1){
    g_1 = InitGraphe(nsom, 1);
  }else{
    g_1 = InitGraphe(nsom, 2*narc);
  }

  for (x = 0; x < nsom; x++) /* pour tout i sommet de g */
    for(p = g->gamma[x]; p != NULL; p = p->next){
      g_1->I[i] = p->som;
      g_1->T[i] = x;
      g_1->poids[i] = g->poids[j];
      AjouteArc(g_1, p->som, x);
      g_1->I[i+1] = x;
      g_1->T[i+1] = p->som;
      g_1->poids[i+1] = g->poids[j];
      AjouteArc(g_1, x, p->som);
      i+=2;
      j++;
    }
  return g_1;
} /* Sym() */


/* ====================================================================== */
/*! \fn booleen * CC(graphe* G, int x)
    \param G : graphe
    \param x : un sommet du graphe
    \return un tableau de bool
    \brief retourne la composante connexe du graphe G contenant le sommet x
*/
booleen * CC(graphe* G, int x){
  booleen *Z;
  graphe *GG = fermetureSymEfficace(G);
  Z = explorationLargeur(GG, x);
  TermineGraphe(GG);
  return Z;
}



/* ====================================================================== */
/*! \fn int Partitionner(int *A, double *T, int p, int r)
    \param A : tableau de int
    \param T : tableau de valeurs
    \param p : indice sur le tableau T
    \param r : indice sur le tableau T
    \return un indice sur A
    \brief partitionne les elements de A entre l'indice p (compris) et l'indice r (compris) 
          en deux groupes : les elements q tq T[A[q]] <= T[A[p]] et les autres.
*/
int Partitionner(int *A, double *T, int p, int r)
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


/* ====================================================================== */
/*! \fn int PartitionStochastique(int *A, double *T, int p, int r)
    \param A : tableau de int
    \param T : tableau de valeurs
    \param p : indice sur le tableau T
    \param r : indice sur le tableau T
    \return un indice sur A
    \brief partitionne les elements de A entre l'indice p (compris) et l'indice r (compris)
          en deux groupes : les elements k tels que T[A[k]] <= T[A[q]] et les autres, 
          avec q tire au hasard dans [p,r].
*/
int PartitionStochastique(int *A, double *T, int p, int r)
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
    \param A (entr�e/sortie) : un tableau d'entiers
    \param T (entr�e) : un tableau de valeurs de type double.
    \param p (entr�e) : indice du d�but de la zone � trier.
    \param r (entr�e) : indice de fin de la zone � trier.
    \brief tri par ordre croissant des valeurs du tableau \b T.
           Le tri s'effectue sur un tableau \b A contenant les index
           des elements de \b T, l'indice \b p (compris) � l'indice \b r (compris).
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


/* ====================================================================== */
/*! \fn int* triAretes(graphePondere *Gp)
    \param Gp : graphe pondéré
    \return un tableau de int
    \brief tri les aretes du graphe par poids et renvoie un pointeur vers le tableau
*/
int* triAretes(graphe *Gp){
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

/* ====================================================================== */
/*! \fn graphe* initGraphMin(graphe* G, double* poidsArbreMin)
    \param G : le graphe utilisé
    \param poidsArbreMin : poids de l'arbre minimum
    \return un graphe arbre de poids minimum
    \brief créé un arbre de poids minimum à partir de graohe utilisé
*/
graphe* initGraphMin(graphe* G, double* poidsArbreMin){
    graphe *T; /* pour stocker l'arbre de poids minimum */
    int *O;          /* tableau pour ranger les index des arcs par ordre croissant de poids*/
    int x, y; // index pour des sommets
    booleen *Z; // pour stocker les composantes connexes
    int i=0, k=0;

    T = InitGraphe(G->nsom, G->nsom-1);
                
    O = triAretes(G); /* O[i] est l'index de la i-�me ar�te par ordre croissant de poids; les extremites de la i-�me aretes sont donc Gp->I[O[i]] et Gp->T[O[i]] */
    
    while(k<((G->nsom)-1)){
        x = G->I[O[i]];
        y = G->T[O[i]];
        Z = CC(T,x);
        if(!Z[y]){
            AjouteArcValue(T, x, y,G->poids[O[i]]);
            k++;
            *poidsArbreMin+= G->poids[O[i]];
        }       
        i++;
        free(Z);
    }
    free(O);
    return T;
}

int isConnexe(graphe* G, int x){
  unsigned char *Z = CC(G,x);
  
  for (int i = 0; i < G->nsom; i++)
  {
    if(Z[i]==0) return 0;
  }

  free(Z);

  return 1;
}