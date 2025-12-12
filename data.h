#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include "avl.h"

/* usine (factory) : stocke les volumes en k.m3 (thousands m3) */
typedef struct {
    char *id;
    double capacite_max_km3;     /* capacité maximale (k.m3) */
    double total_src_km3;        /* somme volumes captés (k.m3) */
    double total_real_km3;       /* volumes arrivant après le tronçon source->usine (k.m3) */
} usine_t;

/* graphe : noeud de distribution */
typedef struct Edge {
    struct NoeudGraphe *fils; /* pointeur vers noeud fils */
    double fuite_pct;          /* pourcentage de fuite sur l'arc */
    struct Edge *suivant;
} Edge;

typedef struct NoeudGraphe {
    char *id;
    Edge *enfants;
} noeud_graphe_t;

/* --- Fonctions de libération exposées pour avl_liberer (CORRECTION) --- */
void liberer_usine_val(void *v);
void liberer_noeud_val(void *v);
/* --------------------------------------------------------------------- */

/* fonctions principales (simplifiées) */
int construire_depuis_csv(const char *chemin, AVL *avl_usines, AVL *avl_noeuds);
int generer_histo(AVL *avl_usines, const char *mode); /* mode: max|src|real -> génère vol_<mode>.dat */
double calculer_fuites(AVL *avl_usines, AVL *avl_noeuds, const char *id_usine); /* retourne fuites en M.m3 (ou -1 si pas trouvée) */

#endif
