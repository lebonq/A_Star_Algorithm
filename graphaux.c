/*! \file graphaux.c
    \brief fonctions auxiliaires
*/
#include "graphaux.h"

/*************************************************
   Fonctions pour la gestion d'une liste lifo
   Michel Couprie 1996
**************************************************/

/* ==================================== */
/*! \fn Lifo * CreeLifoVide(int taillemax)
    \param taillemax (entr�e) : taille maxi de la liste.
    \return une liste Lifo.
    \brief alloue une liste vide.
*/
Lifo * CreeLifoVide(int taillemax)
/* ==================================== */
{
  Lifo * L = (Lifo *)malloc(sizeof(Lifo) + sizeof(int) * (taillemax-1));
  if (L == NULL)
  {   fprintf(stderr, "CreeLifoVide : malloc failed\n");
      exit(0);
  }  
  L->Max = taillemax;
  L->Sp = 0;
  return L;
} // CreeLifoVide()

/* ==================================== */
/*! \fn void LifoFlush(Lifo * L)
    \param L (entr�e) : une liste Lifo.
    \brief r�-initialise L (tous les �l�ments pr�sents sont retir�s).
*/
void LifoFlush(Lifo * L)
/* ==================================== */
{
  L->Sp = 0;
} // LifoFlush()

/* ==================================== */
/*! \fn boolean LifoVide(Lifo * L)
    \param L (entr�e) : une liste Lifo.
    \return TRUE si la liste \b L est vide, FALSE sinon.
    \brief teste si la liste \b est vide.
*/
boolean LifoVide(Lifo * L)
/* ==================================== */
{
  return (boolean)(L->Sp == 0);
} // LifoVide()

/* ==================================== */
/*! \fn int LifoPop(Lifo * L)
    \param L (entr�e) : une liste Lifo.
    \return la valeur se trouvant en d�but de liste.
    \brief retire de la liste \b L la valeur se trouvant au d�but, et retourne cette valeur.
    \warning si la liste \b L est vide, la fonction syst�me \b exit est appel�e.
*/
int LifoPop(Lifo * L)
/* ==================================== */
{
  if (L->Sp == 0)
  {
    fprintf(stderr, "erreur Lifo vide\n");
    exit(1);
  }
  L->Sp -= 1;
  return L->Pts[L->Sp];
} // LifoPop()
  
/* ==================================== */
/*! \fn void LifoPush(Lifo * L, int V)
    \param L (entr�e) : une liste Lifo.
    \brief ins�re au d�but de la liste \b L la valeur \b V
    \warning si la liste \b L est pleine, la fonction syst�me \b exit est appel�e.
*/
void LifoPush(Lifo * L, int V)
/* ==================================== */
{
  if (L->Sp > L->Max - 1)
  {
    fprintf(stderr, "erreur Lifo pleine\n");
    exit(1);
  }
  L->Pts[L->Sp] = V;
  L->Sp += 1;
} // LifoPush()

/* ==================================== */
/*! \fn void LifoPrint(Lifo * L)
    \param L (entr�e) : une liste Lifo.
    \brief affiche le contenu de la liste \b L.
*/
void LifoPrint(Lifo * L)
/* ==================================== */
{
  int i;
  if (LifoVide(L)) {printf("[]"); return;}
  printf("[ ");
  for (i = 0; i < L->Sp; i++)
    printf("%d ", L->Pts[i]);
  printf("]");
} // LifoPrint()

/* ==================================== */
/*! \fn void LifoPrintLine(Lifo * L)
    \param L (entr�e) : une liste Lifo.
    \brief affiche le contenu de la liste \b L, suivi d'un retour ligne.
*/
void LifoPrintLine(Lifo * L)
/* ==================================== */
{
  int i;
  if (LifoVide(L)) {printf("[]\n"); return;}
/*
  printf("Max = %d ; Sp = %d \n", L->Max, L->Sp);
*/
  printf("[ ");
  for (i = 0; i < L->Sp; i++)
    printf("%d ", L->Pts[i]);
  printf("]\n");
} // LifoPrintLine()

/* ==================================== */
/*! \fn void LifoTermine(Lifo * L)
    \param L (entr�e) : une liste Lifo.
    \brief r�cup�re l'espace m�moire occup� par la liste \b L.
*/
void LifoTermine(Lifo * L)
/* ==================================== */
{
  free(L);
} // LifoTermine() 


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
int Partitionner(int *A, TypeCle *T, int p, int r)
/* =============================================================== */
/*
  partitionne les elements de A entre l'indice p (compris) et l'indice r (compris)
  en deux groupes : les elements q tq T[A[q]] <= T[A[p]] et les autres.
*/
{
  int t;
  TypeCle x = T[A[p]];
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
int PartitionStochastique(int *A, TypeCle *T, int p, int r)
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
/*! \fn void TriRapideStochastique (int * A, TypeCle *T, int p, int r)
    \param A (entr�e/sortie) : un tableau d'entiers
    \param T (entr�e) : un tableau de valeurs de type TypeCle.
    \param p (entr�e) : indice du d�but de la zone � trier.
    \param r (entr�e) : indice de fin de la zone � trier.
    \brief tri par ordre croissant des valeurs du tableau \b T.
           Le tri s'effectue sur un tableau \b A contenant les index
           des elements de \b T, l'indice \b p (compris) a l'indice \b r (compris).
*/
void TriRapideStochastique (int * A, TypeCle *T, int p, int r)
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


/*************************************************
	Fonctions de mesure de temps 
    Michel Couprie - ESIEE - juillet 1996
**************************************************/

/* ==================================== */
/*! \fn void start_chrono(chrono *tp)
    \param tp (sortie) : date du d�but du chronom�trage
    \brief d�marre le chronom�trage
*/
void start_chrono(chrono *tp)
/* ==================================== */
{
   struct timezone tzp;
   
   if ( gettimeofday(tp, &tzp) != 0 ) /* code en retour: 0=Ok, -1=erreur */
     fprintf(stderr, "start_chrono() : gettimeofday failed\n");
} // start_chrono()

/* ==================================== */
/*! \fn int read_chrono(chrono *tp)
    \param tp (entr�e) : date du d�but du chronom�trage
    \return nombre de microsecondes �coul�es depuis le d�but
    \brief lecture du chronom�tre
*/
int read_chrono(chrono *tp)
/* ==================================== */
{
   struct timeval tp2;
   struct timezone tzp;
   
   if ( gettimeofday(&tp2, &tzp) != 0 ) /* code en retour: 0=Ok, -1=erreur */
     fprintf(stderr, "read_chrono() : gettimeofday failed\n");
   if (tp->tv_usec >  tp2.tv_usec)
   {
      tp2.tv_usec += 1000000;
      tp2.tv_sec--;
   }
   return( (tp2.tv_sec - tp->tv_sec)*1000000 + (tp2.tv_usec - tp->tv_usec));
} // read_chrono()

/*----------- Exemple d'utilisation : 
void main()
{
  chrono Chrono1;
  int i;
  start_chrono(&Chrono1);
  for(i = 0; i<1000; i++);
  printf("nb de usec ecoulees = %d\n", read_chrono(&Chrono1));

}
*/

/*************************************************
    Fonctions pour la generation de postscript
    Michel Couprie
**************************************************/

/* ========================================== */
void EPSHeader(FILE *fd, double figure_width, double figure_height, double line_width, int font_size )
/* ========================================== */
{
  fprintf(fd, "%%!PS-Adobe-2.0 EPSF-2.0\n");
  fprintf(fd, "%%%%BoundingBox: 0 0 %g %g\n", figure_width, figure_height);
  fprintf(fd, "%g setlinewidth\n", line_width);
  fprintf(fd, "/corps %d def\n", font_size);
  fprintf(fd, "/Fn /Courier findfont corps scalefont def\n");
  fprintf(fd, "/setn {	/CurrentFont Fn def } def\n");
  fprintf(fd, "setn\n");
  fprintf(fd, "/o { CurrentFont setfont show } bind def\n");
} /* EPSHeader() */

/* ========================================== */
void PSHeader(FILE *fd, double figure_width, double figure_height, double line_width, int font_size )
/* ========================================== */
{
  fprintf(fd, "%%!\n");
  fprintf(fd, "%g setlinewidth\n", line_width);
  fprintf(fd, "/corps %d def\n", font_size);
  fprintf(fd, "/Fn /Courier findfont corps scalefont def\n");
  fprintf(fd, "/setn {	/CurrentFont Fn def } def\n");
  fprintf(fd, "setn\n");
  fprintf(fd, "/o { CurrentFont setfont show } bind def\n");
} /* PSHeader() */

/*========================================*/
void PSMove (FILE *fd, double x, double y) 
/*========================================*/
{
  int xx, yy;
  xx = (int)x;
  yy = (int)y;
  fprintf(fd, "%d %d moveto ", xx, yy);
} /* PSMove() */

/*========================================*/
void PSDraw (FILE *fd, double x, double y)
/*========================================*/
{
  int xx, yy;
  xx = (int)x;
  yy = (int)y;
  fprintf(fd, "%d %d lineto ", xx, yy);
} /* PSDraw() */


/*========================================*/
void PSLine (FILE *fd, double xd, double yd, double xf, double yf)
/*========================================*/
{
  fprintf(fd, "newpath ");
  PSMove(fd, xd, yd);
  PSDraw(fd, xf, yf);
  fprintf(fd, "stroke\n");
} /* PSLine() */

/*========================================*/
void PSDrawcircle (FILE *fd, double x, double y, double r)
/*========================================*/
{
  int xx, yy, rr;
  xx = (int)x;
  yy = (int)y;
  rr = (int)r;
  fprintf(fd, "newpath ");
  fprintf(fd, "%d %d %d 0 360 arc ", xx, yy, rr);
  fprintf(fd, "stroke\n");
} /* PSDrawcircle() */

/*========================================*/
void PSDrawdisc (FILE *fd, double x, double y, double r)
/*========================================*/
{
  int xx, yy, rr;
  xx = (int)x;
  yy = (int)y;
  rr = (int)r;
  fprintf(fd, "newpath ");
  fprintf(fd, "%d %d %d 0 360 arc ", xx, yy, rr);
  fprintf(fd, "fill stroke\n");
} /* PSDrawdisc() */

/*========================================*/
void PSString (FILE *fd, double x, double y, char *s)
/*========================================*/
{
  /*int xx, yy, rr;
  xx = (int)x;
  yy = (int)y;*/
  fprintf(fd, "newpath ");
  //fprintf(fd, "%d %d moveto ", xx, yy);
  fprintf(fd, "(%s)o ", s);
  fprintf(fd, "stroke\n");
} /* PSString() */

/* ========================================== */
void PSFooter(FILE *fd)
/* ========================================== */
{
  fprintf(fd, "showpage\n");
} /* PSFooter() */

/*************************************************
    Fonctions pour la representation d'ensembles par tableau de booleens
    Michel Couprie
**************************************************/

/* ====================================================================== */
/*! \fn boolean * EnsembleVide(int n)
    \param n (entr�e) : taille du tableau bool�en.
    \return un tableau de \b n bool�ens, tous �gaux � FALSE.
    \brief alloue et initialise un tableau de \b n bool�ens, repr�sentant l'ensemble vide.
*/
boolean * EnsembleVide(int n)
/* ====================================================================== */
{
  boolean *Ca = (boolean *)calloc(n, sizeof(boolean));
  if (Ca == NULL)
  {   fprintf(stderr, "EnsembleVide : calloc failed\n");
      exit(0);
  }
  return Ca;
} /* EnsembleVide() */



