#include "graphaux.h"
#include "graphes.h"

int main(){
    graphe* G = ReadGraphe("test_dijkstra.graph");

    printf("\n");
    for(int i=0; i<G->nsom; i++){
        //printf("Sommet %d :\n",i); //affichage de tout les sommets
        
        pcell start = G->gamma[i];

        while(start != NULL){
            printf("L'arc va de %d à %d, il vaut %li\n",i,start->som, start->v_arc);//affichage sommet de depart arrive + poids
            start = start->next;
        }
    }

    //printf("Distance entre 0 et 3 est %li\n",get_distance(0,3,G));
    //printf("Distance entre 2 et 1 est %li\n",get_distance(2,1,G));
    return 0;
}