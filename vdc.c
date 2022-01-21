#include "vdc.h"
#include "kruskal.h"
#include <time.h>
#ifdef GRAPHE_INC
#include "graphaux.h"
#include "graphes.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#endif
#define GRAPHE_INC

double poidsArbreMin = 0;
graphe *ArbrePoidsMin;

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
    new_node->next = NULL;
	return new_node;
}

void freeNode(pnode n){
    free(n->listsom);
    free(n);
    return;
}

/* ====================================================================== */
/*! \fn pnode ExtractFirstOpen(pnode* Open)
    \param Open : liste des noeuds "ouverts"
    \return pointeur sur le noeud choisi
    \brief sélectionne un noeud qui minimise la quantité estim_f, le retire de la liste Open et retourne un pointeur dessus
*/
pnode ExtractFirstOpen(pnode* Open){
    
    if(*Open == NULL){
        printf("Erreur LO vide\n");
        exit(-1);
    }

    pnode precedent_min = NULL;
    pnode min = *Open;
    pnode next = (*Open)->next;
    pnode precedent = min;

    long val_min = min->estim_f;
    while(next != NULL){
        if(val_min > next->estim_f){
            val_min = next->estim_f;
            precedent_min = precedent;
            min = next;
        }
        precedent = next;
        next = next->next;
    }

    if(precedent_min == NULL){
        *Open = (min->next);
    }
    else{
        precedent_min->next = min->next;
    }

	return min;
}


/* ====================================================================== */
/*! \fn void PrintSolution(pnode p, graphe *G)
    \param p : pointeur sur le noeud sélectionné à la profondeur n-1
    \param G : le graphe
    \brief affiche la solution à l'écran (le circuit et son coût)
*/
void PrintSolution(pnode P, graphe* G){
    
    printf("\nChemin optimal :\n");
    for (int i = 0; i < (P->len)-1; i++)
    {
        printf("%d -> ",P->listsom[i]);
    }
    printf("%d",P->listsom[P->len-1]);
    printf("\nFin :-)\n");
}

/* ====================================================================== */
/*! \fn int NotInListSom(int s, pnode p)
    \param s : un sommet
    \param p : un noeud
    \return 1 si le sommet n'est pas déjà présent dans le noeud
    \brief regarde si un sommet dans dejà présentdans une node.
*/
int NotInListSom(int s, pnode p){
	int* som = p->listsom;

    //Si on veut un dernier sommet et que celui-ci est le sommet de départ alors on peut y accéder
    if(p->len == p->n-1 && s == p->listsom[0]){
        return 1;
    }

	for(int i = 0; i< p->len; i++){
		if(som[i] == s) return 0;
	}

	return 1;

}

/* ====================================================================== */
/*! \fn long get_distance(int a, int b, graphe* G)
    \param a : un sommet
    \param b : un sommet
    \param G : le graphe utilisé
    \return la distance entre 2 sommets, si l'arc existe dans le graphe
    \brief regarde si une distance existe entre 2 sommets dans le graphe, et la retourne.
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
/*! \fn long arcmin(int s, graphe* G)
    \param s : un sommet
    \param G : le graphe utilisé
    \return : le poids de l'arc minimum partant de ce sommet
    \brief recherche tous les arcs connectés à ce sommet et retourne celui de poids minimum
*/
long arcmin(int s, graphe* G){
    pcell it_som = G->gamma[s];
    if(it_som == NULL) return 0;

    long val = LONG_MAX;
    long test = 0;
    while(it_som != NULL){
        test = it_som->v_arc;
        if( test < val){
            val = test;
        }
        it_som = it_som->next;
    }

    return val;

}

/* ====================================================================== */
/*! \fn long ComputeH(pnode p, graphe* G, int code)
    \param p : un noeud
    \param G : le graphe utilisé
    \param code : le code de l'heuristique (choix parmi différentes possibilités)
    \return : valeur de l'heuristique pour ce noeud
    \brief calcule l'heuristique pour le noeud p
*/
long ComputeH(pnode p, graphe* G, int code){
    
    switch(code){
        // heuristique : g + distance sommet le + proche
        case 1:
            {
                p->estim_f = p->estim_g;
                for (int i = 0; i < G->nsom; i++)
                {
                    if(NotInListSom(i,p)){
                        p->estim_f = p->estim_f + arcmin(i,G); // fonction H
                    }
                }
            }    
            break;
            

        /* arbre poids minimum ici #KrusKalForever*/
        case 2:
            {
                p->estim_f = poidsArbreMin;
                for(int i = 0; i<(p->len)-1;i++){
                    pcell test = (ArbrePoidsMin->gamma[p->listsom[i]]);
                    if(test != NULL){
                        p->estim_f -= test->v_arc;
                    }         
                }
                p->estim_f += p->estim_g;
            }
            break;

        /* abre de poids minimum re calculé */
        case 3:
            {   
                graphe * GSym = fermetureSymEfficace(G);
                int maxSoms = (G->nsom) - (p->len) + 2;
                int maxArcs = GSym->narc;
                graphe* inter = InitGraphe( maxSoms, maxArcs);
                
                int j = 0;
                int correspondance[G->nsom];
                for (int i = 0; i < G->nsom; i++)
                {
                    if( NotInListSom(i,p) || (p->listsom[(p->len)-1] == i) || (p->listsom[0] == i) ){
                        correspondance[i] = j;
                        j++;
                    }else{
                        correspondance[i] = -1;
                    }
                }
                
                int k = 0;
                for (int i = 0; i < maxArcs; i++)
                {
                    
                    int somD = GSym->I[i];
                    int corD = correspondance[somD];
                    int somA = GSym->T[i];
                    int corA = correspondance[somA];
                    int poidsArc = GSym->poids[i];

                    if( (corD != -1) && (corA != -1) && (somD != p->listsom[0]) && ( somA != p->listsom[(p->len)-1] ) ){
                        inter->I[k] = corD;
                        inter->T[k] = corA;
                        inter->poids[k] = poidsArc;
                        AjouteArcValue(inter, corD, corA, poidsArc);
                        k++;
                    }
                    
                }
                
                inter->narc = k;
                bool test = true;
                unsigned char* compCo = CC(inter,0);
                for(int i = 0; i<maxSoms; i++){
                    if(compCo[i] == 0){
                        p->estim_f = LONG_MAX;
                        test = false;
                        break;
                    }
                }
                free(compCo);

                if(test && inter->narc>=1){
                    double poidArbre = 0;
                    graphe* useless = initGraphMin(inter,&poidArbre);
                    p->estim_f = p->estim_g + poidArbre;
                    TermineGraphe(useless); 
                }else{
                    p->estim_f = p->estim_g + 0;
                }
                

                TermineGraphe(inter);
                TermineGraphe(GSym);
                    
                         
            }
            break;


        default:
            printf("Heuristique inconnue\n");
            exit(-1);
    }

    return p->estim_f;
}




/* ====================================================================== */
/*! \fn pnode DevelopNode(pnode p, graphe* G, int choix)
    \param p : un noeud
    \param G : table des distances entre villes
    \param choix : choix de l'heuristique
    \return la liste des nouveaux noeuds créés
    \brief construit la liste des noeuds successeurs sur noeud p dans le graphe
*/
pnode DevelopNode(pnode p, graphe* G, int choix){
    pnode it_res = p;
    long distance = 0;
    for(int i = 0; i<G->nsom; i++){ //on parcours tous les sommets dans le graph
        distance = get_distance(p->listsom[p->len-1],i,G);
        if(NotInListSom(i,p) && distance!=-1) {
            //printf("\nWeight entre %d et %d = %li", p->listsom[p->len-1], i,get_distance(p->listsom[p->len-1],i,G));
            // si il n'est pas deja dans le noeud
            // et il existe un arc

            pnode newnode = AllocNode(p->n);
            
            newnode->len = (p->len)+1;  // +1 sommet
            
            memcpy(newnode->listsom,p->listsom,sizeof(int) * p->n); // liste sommets d'avant
            
            newnode->listsom[newnode->len-1] = i; // +1 sommet

            
            newnode->estim_g = (p->estim_g) + distance ;//+ distance;
            newnode->estim_f = ComputeH(newnode, G, choix) ;
            // MAJ des estimations

            it_res->next = newnode;  // ajoute node next
            it_res = it_res->next; 
        }
        
    }
    
    return p->next;

}

/* ====================================================================== */
/*! \fn void ajoutListe(pnode* L, pnode N)
    \param L : Liste de nodes
    \param N : la node à ajouter
    \brief Ajoute une nouvelle node à la fin de la liste
*/
void ajoutListe(pnode* L, pnode N){

    if(*L == NULL){
        *L = N;
        return;
    }

    pnode ITL = *L;
    int k = 1;
    while(ITL->next != NULL){
        k++;
        ITL=ITL->next;
    }

    ITL->next = N;

}

/* ====================================================================== */
/*! \fn pnode AStar(int n, double *arc, int choix)
    \param n : nombre de villes
    \param G : le graphe utilisé
    \param choix : le choix de l'heuristique (choix parmi différentes possibilités. 1 : heuristique des distances. 2 : arbre de poids minimum)
    \return le noeud de résolution A*
    \brief algorithme A* pour le voyageur de commerce
*/
pnode AStar(int n, graphe *G, int choix){

    // Initialisation

    // Liste ouverte
    pnode LO = AllocNode(n);
    LO->listsom[0] = 0;
    LO->len = 1;
    int TLO = 1; //-> taille LO
    int i =0;
    // Iterateur sur la liste ouverte
    pnode ITLO = NULL;

    while(TLO > 0){
        ITLO = ExtractFirstOpen(&LO); // Si c'est le même que précédement alors break pour sortir de la boucle

        ITLO->next = NULL;
        TLO--;
        
        if(ITLO->len == ITLO->n){ //Condition d'arret

            pnode rem = LO;
            if(rem != NULL){
                pnode rem_next = rem->next;
                while(rem_next!=NULL){
                    freeNode(rem);
                    rem = rem_next;
                    rem_next = rem->next;
                }
                freeNode(rem);
            }
                
            return ITLO;
        }
        
        pnode developement = DevelopNode(ITLO,G,choix); // les nodes suivantes possibles (liste chainée)
        
        freeNode(ITLO);
        
        pnode ITd = developement; // itération sur tt les possibilités
        
        while(ITd != NULL){ // on ajoute les noeuds possibles a LO
        
            if(ITd->listsom[ITd->len - 1] == 0 && ITd->len != n){ //Si Pour une node : Listsom[len-1] = start->som et len != n (pas tt les villes parcourues)
                pnode aFree = ITd;
                ITd = ITd->next; // on passe a la possibilité suivante
                freeNode(aFree);
                continue;
            }

            if(ITd->len == n && ITd->listsom[ITd->len - 1] != 0){ //Si Pour une node len = n et Listsom[n-1] != start->som (pas revenu au debut)
                pnode aFree = ITd;
                ITd = ITd->next; // on passe a la possibilité suivante
                freeNode(aFree);
                continue;
            }

            pnode ajout = ITd;
            ITd = ITd->next; 
            ajout->next = NULL;
        
            ajoutListe(&LO, ajout);

            TLO++;
            i++;
        }
        
    }
    return NULL; //Arrive là si aucune solution
    
}

/* ====================================================================== */
/*! \fn void AfficherArcs(graphe* G)
    \param G : le graphe utilisé
    \brief affiche tous les arcs du graph
*/
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
graphe* randomConnexeGraphe(int n, int m){
    graphe* G = GrapheAleatoire(n,m);
    while(!(isConnexe(G,0))){
        free(G); // eliminer le new graphe si il est pas utile
        G = GrapheAleatoire(n,m);
    }
    return G;
}

/* ====================================================================== */
int main(int argc, char **argv)
/* ====================================================================== */
{
      
    if(argc != 3){
        printf("Usage : ./AEtoile.exe file(null if bench) code(1/2/3)\n");
        exit(-1);
    }
    
    char* graphname = argv[1];
    int code = atoi(argv[2]);
    graphe* G;	
    
    if(!(strcasecmp(graphname,"null"))){
        printf("Mode Bench with code %d\n",code);
        //Test incidence sommets
        for (int j = 2; j < 10; j++){

            char line[50];

            FILE *fout1 = NULL;
            char name[250];
            sprintf(name,"H%d_incidence_som.txt",code);
            fout1 = fopen(name,"a");

            if(fout1==NULL){
                printf("File not open lol\n");
                exit(-1);
            }

            int nb_bench = 1;
            double* values = (double*)malloc(nb_bench*sizeof(double));
            double mean = 0.0;
            int nb_arc = (((double)j * ((double)j - 1)) / 2);
            printf("Pour %d sommets et %d arcs\n",j,nb_arc);

            for (int i = 0; i < nb_bench; i++)
            {    
                struct timeval start,end;
                pnode res = NULL;
                do{
                    G = randomConnexeGraphe(j,nb_arc);
                             
                    if(code == 2){ // si on fait du kruskal
                        ArbrePoidsMin = initGraphMin(G,&poidsArbreMin);
                    }

                    
                    gettimeofday(&start,NULL);
                    res = AStar(G->nsom+1,G,code);
                    gettimeofday(&end,NULL);
                    if(code == 2){ // si on fait du kruskal
                        TermineGraphe(ArbrePoidsMin);
                    }
                }while(res == NULL);

                PrintSolution(res,G);
                freeNode(res);
                TermineGraphe(G);

                values[i] = ((double) ((1000000 * end.tv_sec + end.tv_usec)- (1000000 * start.tv_sec + start.tv_usec)));
                
            }

            //Sum of all values for mean
            double sum = 0.0;
            for (int i = 0; i < nb_bench; i++)
            {
                sum+=values[i];
            }
            mean = sum/nb_bench;//Compute mean value
            printf("Average time taken : %.4f s\n\n",mean/1000000);
        
            sprintf(line,"%d;%d;%f\n",j,nb_arc,mean/1000000);
            fprintf(fout1,"%s", line);
            fclose(fout1);
            free(values);
        }

        // Variation du nb d'arc
        for (int j = 2; j < 10; j++){

            char line[50];

            FILE *fout1 = NULL;
            char name[250];
            sprintf(name,"H%d_incidence_arc.txt",code);
            fout1 = fopen(name,"a");

            if(fout1==NULL){
                printf("File not open lol\n");
                exit(-1);
            }

            for (int nb_arc = j; nb_arc <= (((double)j * ((double)j - 1)) / 2); nb_arc++){
                int nb_bench = 1;
                double* values = (double*)malloc(nb_bench*sizeof(double));
                double mean = 0.0;
                printf("Pour %d sommets et %d arcs\n",j,nb_arc);

                for (int i = 0; i < nb_bench; i++)
                {    
                    struct timeval start,end;
                    pnode res = NULL;
                    do{
                        G = randomConnexeGraphe(j,nb_arc);
                                
                        if(code == 2){ // si on fait du kruskal
                            ArbrePoidsMin = initGraphMin(G,&poidsArbreMin);
                        }

                        
                        gettimeofday(&start,NULL);
                        res = AStar(G->nsom+1,G,code);
                        gettimeofday(&end,NULL);
                        if(code == 2){ // si on fait du kruskal
                            TermineGraphe(ArbrePoidsMin);
                        }
                    }while(res == NULL);

                    PrintSolution(res,G);
                    freeNode(res);
                    TermineGraphe(G);

                    values[i] = ((double) ((1000000 * end.tv_sec + end.tv_usec)- (1000000 * start.tv_sec + start.tv_usec)));
                    
                }

                //Sum of all values for mean
                double sum = 0.0;
                for (int i = 0; i < nb_bench; i++)
                {
                    sum+=values[i];
                }
                mean = sum/nb_bench;//Compute mean value
                printf("Average time taken : %.4f s\n\n",mean/1000000);
            
                sprintf(line,"%d;%d;%f\n",j,nb_arc,mean/1000000);
                fprintf(fout1,"%s", line);
                fclose(fout1);
                free(values);
            }
        }
    }
    else{
        G = ReadGraphe(graphname);
        printf("Mode File with code %d\n",code);
        if(code == 2){ // si on fait du kruskal
            ArbrePoidsMin = initGraphMin(G,&poidsArbreMin);
        }

        struct timeval start,end;
        gettimeofday(&start,NULL);
        
        pnode res = AStar(G->nsom+1,G,code);
        gettimeofday(&end,NULL);
        
        if(res == NULL){ // si il n'y a pas de chemin possible
            printf("Pas de solution pour ce graphe.\n");
            return 0;
        }

        PrintSolution(res,G);
        freeNode(res);
    
        double values = ((double) ((1000000 * end.tv_sec + end.tv_usec)- (1000000 * start.tv_sec + start.tv_usec)));
        printf("Time taken :  %.4f s\n",values/1000000);
        TermineGraphe(G);
        if(code == 2){
            TermineGraphe(ArbrePoidsMin);
        }
    }
    
	return 0;
} // main()
