#define DBL_MAX_VAL 1.79769e+308

#include "vdc.h"
#include <stdio.h>
#include <stdlib.h>

/* ====================================================================== */
/*! \fn pnode AllocNode(int n)
    \param n : nombre de villes
    \return pointeur sur le noeud alloué
    \brief alloue un nouveau noeud
*/
pnode AllocNode(int n){
	pnode new_node = (pnode)malloc(sizeof(node));
	new_node->n = n;
	return new_node;
}

/* ====================================================================== */
/*! \fn pnode ExtractFirstOpen(pnode & Open)
    \param Open : liste des noeuds "ouverts"
    \return pointeur sur le noeud choisi
    \brief sélectionne un noeud qui minimise la quantité estim_f, le retire de la liste Open et retourne un pointeur dessus
*/
pnode ExtractFirstOpen(pnode & Open){
	if(Open == NULL){
		return NULL;
	}
	
	double estim_f_min = Open->estim_f;
	
	pnode n_min = Open;
	pnode n_next = Open->next;
	
	while(n_next != NULL){
		double test = n_next->estim_f;
		if(test < estim_f_min) {
			estim_f_min = test;
			n_min = n_next
		}
		
		n_next = n_next->next;
	}
	
	pnode le_return = n_min;
	n_min = n_min->next;
	
	return le_return;
}


/* ====================================================================== */
/*! \fn void PrintSolution(pnode p, double *arc)
    \param p : pointeur sur le noeud sélectionné à la profondeur n-1
    \param arc : table des distances entre villes
    \brief affiche la solution à l'écran (le circuit et son coût)
*/



/* ====================================================================== */
/*! \fn int NotInListSom(int s, pnode p)
    \param p : un noeud
    \brief teste si s est absent de la liste de sommets du noeud p
*/
int NotInListSom(int s, pnode p){
	int* som = p->listsom;
	for(int i = 0; i< p->n - p->len: i++){
		if(som[i] == s) return 0;
	}
	return 1;

}

/* ====================================================================== */
/*! \fn pnode DevelopNode(pnode p, double *arc)
    \param p : un noeud
    \param arc : table des distances entre villes
    \return la liste des nouveaux noeuds créés
    \brief construit la liste des noeuds successeurs sur noeud p dans le GRP
*/

/* ====================================================================== */
/*! \fn pnode Catenate(pnode p, pnode q)
    \param p, q : listes de noeuds
    \return pointeur sur la liste résultat
    \brief concatène les listes p et q (physiquement), et retourne le pointeur sur la nouvelle liste
*/
pnode Catenate(pnode p, pnode q){
	pnode fin = p;
	while(fin->next != NULL){
		fin = fin->next;
	}
	fin->next = q;
	return p;
}

/* ====================================================================== */
/*! \fn double ComputeH(pnode p, double *arc, int code, int *rest) 
    \param p : un noeud
    \param arc : table des distances entre villes
    \param code : le code de l'heuristique (choix parmi différentes possibilités)
    \param rest : tableau contenant les index des p->n - p->len villes à visiter
    \return : valeur de l'heuristique pour ce noeud
    \brief calcule l'heuristique pour le noeud p
*/

/* ====================================================================== */
/*! \fn void AStar(int n, double *arc, int code)
    \param n : nombre de villes
    \param arc : table des distances entre villes
    \param code : le code de l'heuristique (choix parmi différentes possibilités)
    \brief algorithme A* pour le voyageur de commerce
*/

/* ====================================================================== */
/*! \fn double tailleGRP(int n)
    \param n : nombre de villes
    \return : taille du GRP pour n villes
    \brief estime la taille du GRP pour n villes
*/

/* ====================================================================== */
int main(int argc, char **argv)
/* ====================================================================== */
{
	pnode my_node = AllocNode(10);
	printf("%d\n",my_node->n);
	return 0;
} // main()
