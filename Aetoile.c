#include "graphaux.h"
#include "graphes.h"


int main(){
    graphe* G = ReadGraphe("test_dijkstra.graph");

    printf("\n");
    for(int i=0; i<G->nsom; i++){
        //printf("Sommet %d :\n",i); //affichage de tout les sommets
        
        pcell start = G->gamma[i];

        while(start != NULL){
            printf("L'arc va de %d à %d, il vaut %d\n",i,start->som, start->v_arc);//affichage sommet de depart arrive + poids
            start = start->next;
        }
    }

    return 0;
}

int get_distance(int a, int b){
    
}