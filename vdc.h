/*! \struct node
    \brief structure pour les noeuds du Graphe de Résolution de Problème (GRP)
*/
typedef struct node {
//! estimation de la quantité g
  double estim_g;
//! estimation de la fonction d'évaluation
  double estim_f;
//! chemin sur la carte routière : liste de sommets (villes)
  int *listsom;
//! nombre de villes dans le chemin courant
  int len;
//! nombre total de villes
  int n;
//! suite de la liste ou pointeur NULL
  struct node * next; 
} node;

/*! \var pnode
    \brief pointeur sur un node
*/
typedef node * pnode; 

