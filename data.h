les autres fonctions y'a pas de mof#ifndef DATA_H
#define DATA_H

#include "avl.h"

typedef struct {
    char *id;
    double capacite_max_km3;
    double total_src_km3;
    double total_real_km3;
} usine_t;

typedef struct Edge {
    struct NoeudGraphe *fils;
    double fuite_pct;
    struct Edge *suivant;
} Edge;

typedef struct NoeudGraphe {
    char *id;
    Edge *enfants;
} noeud_graphe_t;

void liberer_usine_val(void *v);
void liberer_noeud_val(void *v);

int construire_depuis_csv(const char *chemin, AVL *avl_usines, AVL *avl_noeuds);
int generer_histo(AVL *avl_usines, const char *mode);
double calculer_fuites(AVL *avl_usines, AVL *avl_noeuds, const char *id);

#endif
