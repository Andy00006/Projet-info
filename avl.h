#ifndef AVL_H
#define AVL_H

#include <stdio.h>

typedef struct Noeud {
    char identifiant[100];
    double capacite_max;
    double volume_capte;
    double volume_traite;
    int hauteur;
    struct Noeud *gauche;
    struct Noeud *droit;
} Noeud;

Noeud* inserer(Noeud* noeud, char* id, double capa, double capt, double trait);
void liberer_arbre(Noeud* racine);
void exporter_infixe_inverse(Noeud* racine, FILE* flux_sortie);
Noeud* rechercher_usine(Noeud* r, char* id);
void executer_mode_leaks(Noeud* racine, char* target_id);

#endif
