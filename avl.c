#include "avl.h"
#include <stdlib.h>
#include <string.h>

// Fonctions utilitaires pour l'équilibre
int obtenir_hauteur(Noeud* n) { return n ? n->hauteur : 0; }
int calculer_equilibre(Noeud* n) { return n ? obtenir_hauteur(n->gauche) - obtenir_hauteur(n->droit) : 0; }
int maximum(int a, int b) { return (a > b) ? a : b; }

// Création d'un nouveau nœud usine
Noeud* creer_noeud(char* id, double capa, double capt, double trait) {
    Noeud* n = malloc(sizeof(Noeud));
    if (!n) exit(1); // Erreur d'allocation
    strncpy(n->identifiant, id, 99);
    n->capacite_max = capa;
    n->volume_capte = capt;
    n->volume_traite = trait;
    n->hauteur = 1;
    n->gauche = n->droit = NULL;
    return n;
}

// Rotation vers la droite
Noeud* rotation_droite(Noeud* y) {
    Noeud* x = y->gauche;
    Noeud* T2 = x->droit;
    x->droit = y;
    y->gauche = T2;
    y->hauteur = maximum(obtenir_hauteur(y->gauche), obtenir_hauteur(y->droit)) + 1;
    x->hauteur = maximum(obtenir_hauteur(x->gauche), obtenir_hauteur(x->droit)) + 1;
    return x;
}

// Rotation vers la gauche
Noeud* rotation_gauche(Noeud* x) {
    Noeud* y = x->droit;
    Noeud* T2 = y->gauche;
    y->gauche = x;
    x->droit = T2;
    x->hauteur = maximum(obtenir_hauteur(x->gauche), obtenir_hauteur(x->droit)) + 1;
    y->hauteur = maximum(obtenir_hauteur(y->gauche), obtenir_hauteur(y->droit)) + 1;
    return y;
}

// Insertion avec équilibrage AVL
Noeud* inserer(Noeud* noeud, char* id, double capa, double capt, double trait) {
    if (!noeud) return creer_noeud(id, capa, capt, trait);

    int comparaison = strcmp(id, noeud->identifiant);
    if (comparaison < 0) 
        noeud->gauche = inserer(noeud->gauche, id, capa, capt, trait);
    else if (comparaison > 0) 
        noeud->droit = inserer(noeud->droit, id, capa, capt, trait);
    else {
        // L'usine existe déjà, on cumule les valeurs
        noeud->capacite_max += capa;
        noeud->volume_capte += capt;
        noeud->volume_traite += trait;
        return noeud;
    }

    // Mise à jour de la hauteur et équilibrage
    noeud->hauteur = 1 + maximum(obtenir_hauteur(noeud->gauche), obtenir_hauteur(noeud->droit));
    int equilibre = calculer_equilibre(noeud);

    // Cas de déséquilibre
    if (equilibre > 1 && strcmp(id, noeud->gauche->identifiant) < 0) return rotation_droite(noeud);
    if (equilibre < -1 && strcmp(id, noeud->droit->identifiant) > 0) return rotation_gauche(noeud);
    if (equilibre > 1 && strcmp(id, noeud->gauche->identifiant) > 0) {
        noeud->gauche = rotation_gauche(noeud->gauche);
        return rotation_droite(noeud);
    }
    if (equilibre < -1 && strcmp(id, noeud->droit->identifiant) < 0) {
        noeud->droit = rotation_droite(noeud->droit);
        return rotation_gauche(noeud);
    }
    return noeud;
}

// Parcours Infixe Inverse pour l'ordre alphabétique décroissant
void exporter_infixe_inverse(Noeud* racine, FILE* flux_sortie) {
    if (racine) {
        exporter_infixe_inverse(racine->droit, flux_sortie);
        fprintf(flux_sortie, "%s;%.3f;%.3f;%.3f\n", 
                racine->identifiant, racine->capacite_max, racine->volume_capte, racine->volume_traite);
        exporter_infixe_inverse(racine->gauche, flux_sortie);
    }
}

void liberer_arbre(Noeud* racine) {
    if (racine) {
        liberer_arbre(racine->gauche);
        liberer_arbre(racine->droit);
        free(racine);
    }
}
