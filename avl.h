#ifndef AVL_H
#define AVL_H

#include <stdio.h>

// Structure AVL (pour Histo et recherche rapide)
typedef struct Noeud {
    char identifiant[100];
    double capacite_max;
    double volume_capte;
    double volume_traite;
    int hauteur;
    struct Noeud *gauche;
    struct Noeud *droit;
} Noeud;

// Structure pour l'Arbre de Distribution (Mode Leaks)
typedef struct NoeudDist {
    char id[100];
    double pct_fuite;
    struct NoeudDist** enfants;
    int nb_enfants;
    int capacite_tab;
} NoeudDist;

// Prototypes
Noeud* inserer(Noeud* noeud, char* id, double capa, double capt, double trait);
Noeud* rechercher_noeud(Noeud* racine, char* id);
void liberer_arbre(Noeud* racine);
void exporter_infixe_inverse(Noeud* racine, FILE* flux_sortie);

#endif

