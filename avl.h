#ifndef AVL_H
#define AVL_H

#include <stdio.h>

// Structure représentant une usine dans l'arbre équilibré
typedef struct Noeud {
    char identifiant[100];
    double capacite_max;
    double volume_capte;
    double volume_traite;
    int hauteur;
    struct Noeud *gauche;
    struct Noeud *droit;
} Noeud;

// Prototypes des fonctions de gestion de l'arbre
Noeud* inserer(Noeud* noeud, char* id, double capa, double capt, double trait);
void liberer_arbre(Noeud* racine);
void exporter_infixe_inverse(Noeud* racine, FILE* flux_sortie);

#endif
