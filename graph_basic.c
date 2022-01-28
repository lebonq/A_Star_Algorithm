/*! \file graph_basic.c
    \brief fonctions de base pour la manipulation de graphes
*/
/*
      Michel Couprie, septembre 1999
*/
#include "graphaux.h"
#include "graphes.h"
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

/* ====================================================================== */
/* ====================================================================== */
/* FONCTIONS SUR LES LISTES CHAINEES DE SOMMETS */
/* ====================================================================== */
/* ====================================================================== */

/* ====================================================================== */
/*! \fn pcell AlloueCell(pcell * plibre)
    \param plibre (entr�e) : pointeur sur une liste cha�nee de cellules libres.
    \return pointeur sur une cellule.
    \brief retire la premiere cellule de la liste point�e par plibre et retourne un pointeur sur cette cellule.
*/
pcell AlloueCell(pcell * plibre)
/* ====================================================================== */
{
  pcell p;
  if (*plibre == NULL) 
  {
    fprintf(stderr, "AlloueCell : plus de cellules libres\n");
    exit(0);
  }
  p = *plibre;
  *plibre = (*plibre)->next;
  return p;
} /* AlloueCell() */

/* ====================================================================== */
/*! \fn void LibereCell(pcell * plibre, pcell p)
    \param plibre (entr�e) : pointeur sur une liste cha�nee de cellules libres.
    \param p (entr�e) : pointeur sur une cellule.
    \brief ins�re la cellule p au d�but de la liste point�e par 'plibre'. 
*/
void LibereCell(pcell * plibre, pcell p)
/* ====================================================================== */
{
  p->next = *plibre;
  *plibre = p;
} /* LibereCell() */

/* ====================================================================== */
/*! \fn void RetireTete(pcell * plibre, pcell * pliste)
    \param plibre (entr�e) : pointeur sur une liste cha�nee de cellules libres.
    \param pliste (entr�e) : pointeur sur une liste.
    \brief retire la premi�re cellule de la liste 'pliste'. La cellule est cha�nee � la liste 'plibre'. 
*/
void RetireTete(pcell * plibre, pcell * pliste)
/* ====================================================================== */
{
  pcell p;
  p = (*pliste)->next;
  LibereCell(plibre, *pliste);
  *pliste = p;
} /* RetireTete() */

/* ====================================================================== */
/*! \fn void AjouteTete(pcell * plibre, pcell * pliste, int a, TYP_VARC v)
    \param plibre (entr�e) : pointeur sur une liste cha�nee de cellules libres.
    \param pliste (entr�e) : pointeur sur une liste.
    \param a (entr�e) : un sommet.
    \param v (entr�e) : une valeur.
    \brief ajoute une cellule contenant le sommet 'a' et la valeur 'v' en t�te de la liste 'pliste'. La cellule est prise dans la liste 'plibre'. 
*/
void AjouteTete(pcell * plibre, pcell * pliste, int a, TYP_VARC v)
/* ====================================================================== */
{
  pcell p;
  p = AlloueCell(plibre);
  p->next = *pliste;
  p->som = a;
  p->v_arc = v;
  *pliste = p;
} /* AjouteTete() */

/* ====================================================================== */
/*! \fn int EstDansListe(pcell p, int a) 
    \param p (entr�e) : une liste cha�nee de successeurs.
    \param a (entr�e) : un sommet.
    \return bool�en.
    \brief retourne 1 si le sommet 'a' se trouve dans la liste 'p', 0 sinon. 
*/
int EstDansListe(pcell p, int a) 
/* ====================================================================== */
{
  for (; p != NULL; p = p->next)
    if (p->som == a) return 1;
  return 0;
} /* EstDansListe() */

/* ====================================================================== */
/* ====================================================================== */
/* FONCTIONS D'ALLOCATION / LIBERATION POUR UN GRAPHE */
/* ====================================================================== */
/* ====================================================================== */

/* ====================================================================== */
/*! \fn graphe * InitGraphe(int nsom, int nmaxarc)
    \param   nsom (entr�e) : nombre de sommets.
    \param nmaxarc (entr�e) : nombre maximum d'arcs.
    \return un graphe.
    \brief alloue la memoire n�cessaire pour repr�senter un graphe a 'nsom' sommets,
              poss�dant au maximum 'nmaxarc' arcs. 
              Retourne un pointeur sur la structure allou�e. 
*/
graphe * InitGraphe(int nsom, int nmaxarc)
/* ====================================================================== */
{
  graphe * g;
  int i;
  
  g = (graphe *)malloc(sizeof(graphe));
  if (g == NULL)
  {   fprintf(stderr, "InitGraphe : malloc failed\n");
      exit(0);
  }

  g->reserve = (cell *)malloc(nmaxarc * sizeof(cell));
  if (g->reserve == NULL)
  {   fprintf(stderr, "InitGraphe : malloc failed\n");
      exit(0);
  }

  g->gamma = (pcell *)calloc(nsom, sizeof(pcell)); /* calloc met a 0 la memoire allouee */
  if (g->gamma == NULL)
  {   fprintf(stderr, "InitGraphe : calloc failed\n");
      exit(0);
  }

  g->tete = (int *)malloc(nmaxarc * sizeof(int));
  g->queue = (int *)malloc(nmaxarc * sizeof(int));
  if ((g->tete == NULL) || (g->tete == NULL))
  {   fprintf(stderr, "InitGraphe : malloc failed\n");
      exit(0);
  }

  g->v_arcs = (TYP_VARC *)malloc(nmaxarc * sizeof(TYP_VARC));
  if (g->v_arcs == NULL)
  {   fprintf(stderr, "InitGraphe : malloc failed\n");
      exit(0);
  }

  g->v_sommets = (TYP_VSOM *)malloc(nsom * sizeof(TYP_VSOM));
  if (g->v_sommets == NULL)
  {   fprintf(stderr, "InitGraphe : malloc failed\n");
      exit(0);
  }

  g->x = (double *)malloc(nsom * sizeof(double));
  g->y = (double *)malloc(nsom * sizeof(double));
  if ((g->x == NULL) || (g->y == NULL))
  {   fprintf(stderr, "InitGraphe : malloc failed\n");
      exit(0);
  }

  g->I = (int*)calloc(nmaxarc, sizeof(int));
  g->T = (int*)calloc(nmaxarc, sizeof(int));
  g->poids= (double*)calloc(nmaxarc, sizeof(double));

  g->nsom = nsom;
  g->nmaxarc = nmaxarc;
  g->narc = 0;

  /* construit la liste initiale de cellules libres */
  for (i = 0; i < nmaxarc - 1; i++)
    (g->reserve+i)->next = g->reserve+i+1;
  (g->reserve+i)->next = NULL;
  g->libre = g->reserve;  

  g->nomsommet = NULL;
  
  return g;
} /* InitGraphe() */

/* ====================================================================== */
/*! \fn void TermineGraphe(graphe * g)
    \param g (entr�e) : un graphe.
    \brief lib�re la memoire occup�e par le graphe g. 
*/
void TermineGraphe(graphe * g)
/* ====================================================================== */
{
  
  int i, n = g->nsom;
  
  free(g->reserve);
  if (g->gamma) free(g->gamma);
  if (g->tete) { free(g->tete); free(g->queue); }
  if (g->v_arcs) free(g->v_arcs);
  if (g->v_sommets) free(g->v_sommets);
  if (g->nomsommet)
  {
    for (i = 0; i < n; i++) free(g->nomsommet[i]);
    free(g->nomsommet);
  }
  
  free(g->x);
  free(g->y);
  free(g->I);
  free(g->T);
  free(g->poids);
  
  free(g);
} /* TermineGraphe() */


/* ====================================================================== */
/* ====================================================================== */
/* FONCTIONS D'ENTREE / SORTIE FICHIER POUR UN GRAPHE */
/* ====================================================================== */
/* ====================================================================== */

/* ====================================================================== */
/*! \fn graphe * ReadGraphe(char * filename)
    \param   filename (entr�e) : nom du fichier graphe.
    \return un graphe.
    \brief Lit les donn�es d'un graphe dans le fichier filename, retourne un pointeur sur la structure graphe construite. 
*/
graphe * ReadGraphe(char * filename)
/* ====================================================================== */
{
#define TAILLEBUF 4096
  graphe * g;
  int i, n, m, t, q;
  char buf[TAILLEBUF];
  char *ret;

  FILE *fd = NULL;

  fd = fopen(filename,"r");
  if (!fd)
  {
    fprintf(stderr, "ReadGraphe: file not found: %s\n", filename);
    return NULL;
  }

  fscanf(fd, "%d %d\n", &n, &m);
  g = InitGraphe(n, m);
  do
  {
    ret = fgets(buf, TAILLEBUF, fd);
    if ((ret != NULL) && (strncmp(buf, "noms sommets", 12) == 0))
    {
      g->nomsommet = (char **)malloc(n * sizeof(char *));
      if (g->nomsommet == NULL)
      {   fprintf(stderr, "ReadGraphe : malloc failed\n");
          exit(0);
      }
      for (i = 0; i < n; i++)
      {
        fscanf(fd, "%d\n", &t);
        fgets(buf, TAILLEBUF, fd);
        g->nomsommet[t] = (char *)malloc((strlen(buf)+1) * sizeof(char));
        if (g->nomsommet[t] == NULL)
        {   fprintf(stderr, "ReadGraphe : malloc failed\n");
            exit(0);
        }
        strcpy(g->nomsommet[t], buf);
      } /* for (i = 0; i < n; i++) */
    } /* if ((ret != NULL) && (strcmp(buf, "noms sommets") == 0)) */
    else if ((ret != NULL) && (strncmp(buf, "coord sommets", 13) == 0))
    {
      double x, y;
      for (i = 0; i < n; i++)  
      {
        fscanf(fd, "%d %lf %lf\n", &t, &x, &y);
        g->x[t] = x; 
        g->y[t] = y; 
      }
    } /*  if ((ret != NULL) && (strncmp(buf, "arcs values", 13) == 0)) */
    else if ((ret != NULL) && (strncmp(buf, "arcs values", 11) == 0))
    {
      double v;
      for (i = 0; i < m; i++)  
      {
        fscanf(fd, "%d %d %lf\n", &t, &q, &v);
        AjouteArcValue(g, t, q, (TYP_VARC)v);
        g->I[i] = t;
        g->T[i] = q;
        g->poids[i] = v;
      }
    } /*  if ((ret != NULL) && (strncmp(buf, "arcs values", 11) == 0)) */
    else if ((ret != NULL) && (strncmp(buf, "arcs", 4) == 0))
    {
      for (i = 0; i < m; i++)  
      {
        fscanf(fd, "%d %d\n", &t, &q);
        AjouteArc(g, t, q);
      }
    } /*  if ((ret != NULL) && (strncmp(buf, "arcs", 4) == 0)) */
  } while (ret != NULL);

  return g;
} /* ReadGraphe() */

/* ====================================================================== */
/* ====================================================================== */
/* FONCTIONS DE MANIPULATION DES ARCS (APPLICATION GAMMA UNIQUEMENT) */
/* ====================================================================== */
/* ====================================================================== */

/* ====================================================================== */
/*! \fn void AjouteArc(graphe * g, int i, int s)
    \param g (entr�e/sortie) : un graphe.
    \param i (entr�e) : extr�mit� initiale de l'arc.
    \param s (entr�e) : extr�mit� finale de l'arc.
    \brief ajoute l'arc (i,s) au graphe g (application gamma seulement). 
*/
void AjouteArc(graphe * g, int i, int s)
/* ====================================================================== */
{
  AjouteTete(&(g->libre), &(g->gamma[i]), s, 0);
  g->narc++;
} /* AjouteArc() */

/* ====================================================================== */
/*! \fn void AjouteArcValue(graphe * g, int i, int s, TYP_VARC v)
    \param g (entr�e/sortie) : un graphe.
    \param i (entr�e) : extr�mit� initiale de l'arc.
    \param s (entr�e) : extr�mit� finale de l'arc.
    \param v (entr�e) : une valeur pour l'arc.
    \brief ajoute l'arc (i,s) au graphe g (application gamma seulement). 
*/
void AjouteArcValue(graphe * g, int i, int s, TYP_VARC v)
/* ====================================================================== */
{
  AjouteTete(&(g->libre), &(g->gamma[i]), s, v);
  g->narc++;
} /* AjouteArcValue() */

/* ====================================================================== */
/*! \fn void RetireArc(graphe * g, int i, int s)
    \param g (entr�e/sortie) : un graphe.
    \param i (entr�e) : un sommet de g.
    \param s (entr�e) : un sommet de g.
    \brief retire l'arc (i,s) du graphe g (application gamma seulement),
              si celui-ci etait pr�sent. Sinon, pas d'action. 
*/
void RetireArc(graphe * g, int i, int s)
/* ====================================================================== */
{
  pcell * pliste;
  pliste = g->gamma + i;
  while ((*pliste != NULL) && (((*pliste)->som) != s))
    pliste = &((*pliste)->next);
  if (*pliste != NULL) RetireTete(&(g->libre), pliste);
  g->narc--;
} /* RetireArc() */

/* ====================================================================== */
/*! \fn int PopSuccesseur(graphe * g, int i)
    \param g (entr�e/sortie) : un graphe.
    \param i (entr�e) : un sommet de g.
    \brief retire un arc (i,s) du graphe g (application gamma seulement),
           et retourne le sommet s
    \warning le sommet i doit avoir au moins un successeur (pas de v�rification)
*/
int PopSuccesseur(graphe * g, int i)
/* ====================================================================== */
{
  int s = g->gamma[i]->som;
  RetireTete(&(g->libre), &(g->gamma[i]));
  return s;
} /* PopSuccesseur() */

/* ====================================================================== */
/*! \fn int EstSuccesseur(graphe *g, int i, int s) 
    \param g (entr�e) : un graphe.
    \param i (entr�e) : un sommet de g.
    \param s (entr�e) : un sommet de g.
    \return bool�en.
    \brief retourne 1 si le sommet 's' est un successeur du sommet 'i', 0 sinon.
*/
int EstSuccesseur(graphe *g, int i, int s) 
/* ====================================================================== */
{
  return EstDansListe(g->gamma[i], s);
} /* EstSuccesseur() */

/* ====================================================================== */
/* ====================================================================== */
/* FONCTIONS DE GENERATION DE GRAPHES */
/* ====================================================================== */
/* ====================================================================== */

/* ====================================================================== */
/*! \fn graphe * GrapheAleatoire(int nsom, int narc, int code)
    \param nsom (entr�e) : nombre de sommets.
    \param narc (entr�e) : nombre d'arcs.
    \param code (entrée) : disposition des arcs
    \return un graphe.
    \brief retourne un graphe al�atoire � 'nsom' sommets et 'narc' arcs. 
              Le graphe est antisym�trique et sans boucle.
              Le nombre d'arcs doit �tre <= nsom (nsom - 1) / 2.
              Les arcs sont pond�r�s (valeur al�atoire entre 0.0 et 1.0).
    \warning la m�thode employ�e ici est na�ve, tres inefficace du point de vue
              temps de calcul et ne garantit aucune propri�t� statistique. 
*/
graphe * GrapheAleatoire(int nsom, int narc, int code)
/* ====================================================================== */
{
  graphe * g;
  int i, j, m;
  double mmax = ((double)nsom * ((double)nsom - 1)) / 2;
  srand(time(NULL)+getpid());
  if (narc > mmax)
  {
    fprintf(stderr, "GrapheAleatoire : pas plus de %g arcs pour %d sommets\n", 
                     mmax, nsom);
    exit(0);
  }

  if ((mmax >= (1<<31)) || (narc <= ((int)mmax) / 2))
  {
    g = InitGraphe(nsom, narc);
    while (narc > 0)
    {
      narc--;
      do
      {
        i = rand() % nsom;
        j = rand() % nsom;
      } while ((i == j) || EstSuccesseur(g, i, j) || EstSuccesseur(g, j, i));
      AjouteArc(g, i, j);
      g->I[narc] = i;
      g->T[narc] = j;
      g->tete[narc] = i;
      g->queue[narc] = j;
      if (g->v_arcs){
        g->v_arcs[narc] = (TYP_VARC)(rand()*100.0);
        g->poids[narc] = g->v_arcs[narc];
      } 
    }
  }
  else /* on part du graphe complet et on retire des arcs au hasard */
  {
    /* construit un graphe complet antisymetrique sans boucle */
    g = InitGraphe(nsom, (int)mmax);
    for (i = 0; i < nsom ; i++)
      for (j = i+1; j < nsom; j++){
        AjouteArc(g, i, j);  
      }
    
    // autre

    if(code==1){

     int** listeNbArcs = (int**)calloc(sizeof(int*),nsom);

      for(int k = 0; k<nsom; k++){
        listeNbArcs[k] = (int*)calloc(sizeof(int),nsom);
      }

      i = 0;
      j = 0;
      int d = 0;
      for(int na = 0; na<narc; na++){
        int nbmin = INT_MAX;
        for(d = 0; d<nsom; d++){
          int nbactuel;
          for(int a=0; a<nsom;a++){
            nbactuel+=listeNbArcs[d][a];
          }
          if(nbactuel <= nbmin) nbmin = nbactuel;        
        } // nb min d'arcs partant d'un sommet

        int nbSomAConsiderer = 0;
        for(d = 0; d<nsom; d++){
          int nbactuel = 0;
          for(int a=0; a<nsom;a++){
            nbactuel+=listeNbArcs[d][a];
          }
          if(nbactuel == nbmin) nbSomAConsiderer++;        
        } // nb de sommet qui possiblement gagnent un arc ce tour

        int listeSomAConsiderer[nbSomAConsiderer];
        int c = 0;
        for(d = 0; d<nsom; d++){
          int nbactuel;
          for(int a=0; a<nsom;a++){
            nbactuel+=listeNbArcs[d][a];
          }
          if(nbactuel == nbmin){
            listeSomAConsiderer[c] = d;
            c++;
          }
        } // liste des sommets qui peuvent gagner un arc

        // 2 random sur listeSomAConsiderer

        int sommet1 = listeSomAConsiderer[rand() % nbSomAConsiderer];
        int sommet2 = listeSomAConsiderer[rand() % nbSomAConsiderer];
        while(EstSuccesseur(g,sommet1,sommet2) || EstSuccesseur(g,sommet2,sommet1)){
          sommet2 = listeSomAConsiderer[rand() % nbSomAConsiderer];
        }
        AjouteArc(g,sommet1,sommet2);
        listeNbArcs[sommet1][sommet2] = 1;
        listeNbArcs[sommet2][sommet1] = 1;
      }
    
      for(int k = 0; k<nsom; k++){
        free(listeNbArcs[k]);
      }
      free(listeNbArcs);
      return g;
    }
    
    // autre 2
    if(code == 2){
      int k;
      for(k = 0; k<nsom-1; k++){
        AjouteArc(g,k,k+1);
      }
      AjouteArc(g,k,0);

      int nplus = narc - nsom;
      for(int y = 0; y<nplus;y++){
        int sommet1 = rand() % nsom;
        int sommet2 = rand() % nsom;
        while(EstSuccesseur(g,sommet1,sommet2) || EstSuccesseur(g,sommet2,sommet1)){
          sommet2 = rand() % nsom;
        }
        AjouteArc(g,sommet1,sommet2);
      }
      
      return g;
    }

               

    /* retire mmax - narc arcs */
    m = (int)mmax;
    while (m > narc)
    {
      m--;
      do
      {
        i = rand() % nsom;
        j = rand() % nsom;
      } while ((i == j) || !EstSuccesseur(g, min(i,j), max(i,j)));
      RetireArc(g, min(i,j), max(i,j));
    }

    /* rajoute la liste des arcs et les poids */
    m = 0;
    for (i = 0; i < nsom ; i++)
      for (j = i+1; j < nsom; j++)
        if (EstSuccesseur(g, i, j))
	      {
          g->tete[m] = i;
          g->queue[m] = j;
          if (g->v_arcs) g->v_arcs[narc] = (TYP_VARC)(rand()*100.0);
          m++;
	      }
  }

  return g;
} /* GrapheAleatoire() */
