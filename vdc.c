#define DBL_MAX_VAL 1.79769e+308

#include "vdc.h"
#include "graphaux.h"
#include "graphes.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ====================================================================== */
/*! \fn pnode AllocNode(int n)
    \param n : nombre de villes
    \return pointeur sur le noeud alloué
    \brief alloue un nouveau noeud
*/
pnode AllocNode(int n){
	pnode new_node = (pnode)calloc(1,sizeof(node));
	new_node->n = n;
    new_node->listsom = (int*)calloc(n,sizeof(int));
    new_node->estim_g = 0;
    new_node->estim_f = 0;
    new_node->len = 0;
	return new_node;
}

/* ====================================================================== */
/*! \fn pnode ExtractFirstOpen(pnode & Open)
    \param Open : liste des noeuds "ouverts"
    \return pointeur sur le noeud choisi
    \brief sélectionne un noeud qui minimise la quantité estim_f, le retire de la liste Open et retourne un pointeur dessus
*/
pnode ExtractFirstOpen(pnode Open){
	if(Open == NULL){
		return NULL;
	}
	
	long estim_f_min = Open->estim_f;
	
	pnode n_min = Open;
	pnode n_next = Open->next;
	
	while(n_next != NULL){
		long test = n_next->estim_f;
		if(test < estim_f_min) {
			estim_f_min = test;
			n_min = n_next;
		}
		
		n_next = n_next->next;
	}
	
	pnode le_return = AllocNode(n_min->n);
    memcpy(le_return,n_min,sizeof(*n_min));
	n_min = n_min->next;
	
	return le_return;
}


/* ====================================================================== */
/*! \fn void PrintSolution(pnode p, graphe *G)
    \param p : pointeur sur le noeud sélectionné à la profondeur n-1
    \param G : le graphe
    \brief affiche la solution à l'écran (le circuit et son coût)
*/
void PrintSolution(pnode P, graphe* G){
    
    printf("Chemin optimal :\n");
    for (int i = 0; i < P->len; i++)
    {
        printf("%d -> ",P->listsom[i]);
    }
    printf("\nFin :-)\n");
}
/* ====================================================================== */
/*! \fn int NotInListSom(int s, pnode p)
    \param p : un noeud
    \brief teste si s est absent de la liste de sommets du noeud p
*/
int NotInListSom(int s, pnode p){
	int* som = p->listsom;
	for(int i = 0; i< p->len; i++){
		if(som[i] == s) return 0;
	}

	return 1;

}

/* ====================================================================== */
/*! \fn double get_distance(double a, int b, graphe* G)
    \param a : un sommet
    \param b : un sommet
    \param G : le graph
    \return la distance entre 2 sommets, si l'arc existe
    \brief regarde si une distance existe entre 2 sommets, et la retourne.
*/
long get_distance(int a, int b, graphe* G){
    pcell cellA = G->gamma[a];
    pcell cellB = G->gamma[b];

    //On cherche l'arc (a,b)
    pcell start = cellA;
    while(start != NULL){
        if( start->som == b){
            return start->v_arc;
        }
        start = start->next;
    }

    //Si on est ici alors le on cherche l'arc (b,a)
    start = cellB;
    while(start != NULL){
        if( start->som == a){
            return start->v_arc;
        }
        start = start->next;
    }

    //sinon on retourne -1 car l'arc n'existe pas
    return -1;
}

/* ====================================================================== */
/*! \fn double get_vol(int a, int b, graphe* G)
    \param a : un sommet
    \param b : un sommet
    \param G : le graph
    \return la distance entre 2 sommets, à vol d'oiseau
    \brief retourne la distance a vol d'oiseau entre 2 points.
*/
double get_vol(int a, int b, graphe* G){
    return 6371 * acos(cos(G->y[a] - G->y[b]) * cos(G->x[a]) * cos(G->x[b]) + (sin(G->x[a]) * sin(G->x[b])));
}


long arcmin(int s, graphe* G){
    pcell it_som = G->gamma[s];
    long val = LONG_MAX;

    while(it_som != NULL){
        if(get_distance(s,it_som->som,G) < val){
            val = get_distance(s,it_som->som,G);
        }
        it_som = it_som->next;
    }

    return val;

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
long ComputeH(pnode p, graphe* G, int code, int *rest){
    p->estim_f = p->estim_g;
    switch(code){
        // heuristique : g + distance sommet le + proche
        case 1:
            for (int i = 0; i < G->nsom; i++)
            {
                if(NotInListSom(i,p)){
                    p->estim_f = p->estim_f + arcmin(p->listsom[p->len-1],G); // fonction H
                }
            }
            
            break;




        /* arbre poids minimum ici #KrusKalForever*/
        case 2:


            break;

        default:
            break;
    }

    return p->estim_f;
}


/* ====================================================================== */
/*! \fn pnode DevelopNode(pnode p, graphe* G, int som_end)
    \param p : un noeud
    \param G : table des distances entre villes
    \return la liste des nouveaux noeuds créés
    \brief construit la liste des noeuds successeurs sur noeud p dans le GRP
*/
pnode DevelopNode(pnode p, graphe* G){
    pnode it_res = p;

    for(int i = 0; i<G->nsom; i++){ //on parcours tous les sommets dans le graph
        if(NotInListSom(i,p) && get_distance(p->listsom[p->len-1],i,G)!=-1) {
            printf("Weight entre %d et %d = %li\n", p->listsom[p->len-1], i,get_distance(p->listsom[p->len-1],i,G));
            // si il n'est pas deja dans le noeud
            // et il existe un arc

            pnode newnode = AllocNode(p->n);
            //memcpy(newnode, p, sizeof(*p)); // node d'avant

            
            newnode->len = (p->len)+1;  // +1 sommet

            memcpy(newnode->listsom,p->listsom,sizeof(int) * p->n); // liste sommets d'avant
            newnode->listsom[newnode->len-1] = i; // +1 sommet

            newnode->estim_g = newnode->estim_g + get_distance(p->listsom[p->len-1],i,G);
            newnode->estim_f = ComputeH(newnode, G, 1, NULL) ;
            // MAJ des estimations

            it_res->next = (pnode)calloc(1,sizeof(*newnode));
            memcpy(it_res->next,newnode,sizeof(*newnode));  // ajoute node next
            //it_res->next = newnode;
            it_res = it_res->next;

        }
    }
    return p->next;

}



/* ====================================================================== */
/*! \fn pnode Catenate(pnode p, pnode q)
    \param p, q : listes de noeuds
    \return pointeur sur la liste résultat
    \brief concatène les listes p et q (physiquement), et retourne le pointeur sur la nouvelle liste
*/
pnode Catenate(pnode p, pnode q){
    pnode it_res = p;
    while(p->next != NULL){
        it_res = it_res->next;
    }
    it_res->next = q;

	return p;
}

void appendTo(pnode L, pnode N, int* size){
    L->next = N;
    L = L->next;
    *size = *size + 1;
}

/* ====================================================================== */
/*! \fn pnode AStar(int n, double *arc, int code)
    \param n : nombre de villes
    \param arc : table des distances entre villes
    \param code : le code de l'heuristique (choix parmi différentes possibilités)
    \brief algorithme A* pour le voyageur de commerce
*/
pnode AStar(int n, graphe *G, int code){

    // Liste ouverte
    pnode LO = AllocNode(n);
    int TLO = 0; //-> taille LO

    // Iterateur sur la liste ouverte
    pnode ITLO = LO;
    
    // Initialisation
    
    pnode N = AllocNode(n); // nouveau noeud pour le départ
    N->listsom[0] = 0;
    N->len= 1;

    appendTo(ITLO,N,&TLO); // ajoute N a la LO

    while(TLO > 0 || (N->len == n && N->listsom[N->len - 1] != 0)){ //On s'arrete quand pour la node actuelle len = n et listsom[len-1] = start->som et estimf < à tous les autres (autrement dit si Extractmachin ressort la même node que celle actuelle)

        pnode developement = DevelopNode(N,G); // les nodes suivantes possibles (liste chainée)
        pnode ITd = developement; // itération sur tt les possibilités
        
        
        while(ITd->next != NULL){ // on ajoute les noeuds possibles a LO
            if(ITd->listsom[ITd->len - 1] == 0 && ITd->len != n){ //Si Pour une node : Listsom[len-1] = start->som et len != n (pas tt les villes parcourues)
                ITd = ITd->next; // on passe a la possibilité suivante
                continue;
            }

            if(ITd->len == n && ITd->listsom[ITd->len - 1] != 0){ //Si Pour une node len = n et Listsom[n-1] != start->som (pas revenu au debut)
                ITd = ITd->next; // on passe a la possibilité suivante
                continue;
            }

            appendTo(ITLO, ITd->next,&TLO);
            ITd = ITd->next;
            
        }
        
        N->next = ExtractFirstOpen(LO);
        N = N->next;
        N->len = N->len + 1;
        TLO--;
    }
    
    return N;
    
    //N la node final
}

void AfficherArcs(graphe* G){
    for (int i = 0; i < G->nsom-1; i++)
    {
        pcell start = G->gamma[i];
        while (start != NULL)
        {
            printf("Arc de %d à %d = %li\n",i, start->som,start->v_arc);
            start = start->next;
        }
        
    }
}
    


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
  
    char graphname[50] = "carte_france.graph";
	graphe* G = ReadGraphe(graphname);

    //AfficherArcs(G);

    pnode start = AllocNode(G->nsom);
    start->listsom[0] = 0;
    start->len = 1;
    pnode dev = DevelopNode(start,G);

    pnode d = AllocNode(G->nsom);
    memcpy(d,dev,sizeof(*dev));

    while(d != NULL){
       // printf("Listsom : ");
        for(int i = 0;i<(d->len);i++){
           // printf("Itération %d = %d, ",i,d->listsom[i]);
        }
        
        //printf("\nlen = %d\n",d->len);
        d = d->next;
        
    }

    

    pnode res = AStar(G->nsom,G,0);
    PrintSolution(res,G);
	return 0;
} // main()
