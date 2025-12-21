#include "avl.h"
#include <stdlib.h>
#include <string.h>

int obtenir_hauteur(Noeud* n) {
    if (n == NULL) {
        return 0;
    }
    return n->hauteur;
}

int calculer_equilibre(Noeud* n) {
    if (n == NULL) {
        return 0;
    }
    return obtenir_hauteur(n->gauche) - obtenir_hauteur(n->droit);
}

int maximum(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

Noeud* creer_noeud(char* id, double capa, double capt, double trait) {
    Noeud* n = malloc(sizeof(Noeud));
    if (!n) exit(1); 
    strncpy(n->identifiant, id, 99);
    n->capacite_max = capa;
    n->volume_capte = capt;
    n->volume_traite = trait;
    n->hauteur = 1;
    n->gauche = n->droit = NULL;
    return n;
}

Noeud* rotation_droite(Noeud* y) {
    Noeud* x = y->gauche;
    Noeud* T2 = x->droit;
    x->droit = y;
    y->gauche = T2;
    y->hauteur = maximum(obtenir_hauteur(y->gauche), obtenir_hauteur(y->droit)) + 1;
    x->hauteur = maximum(obtenir_hauteur(x->gauche), obtenir_hauteur(x->droit)) + 1;
    return x;
}

Noeud* rotation_gauche(Noeud* x) {
    Noeud* y = x->droit;
    Noeud* T2 = y->gauche;
    y->gauche = x;
    x->droit = T2;
    x->hauteur = maximum(obtenir_hauteur(x->gauche), obtenir_hauteur(x->droit)) + 1;
    y->hauteur = maximum(obtenir_hauteur(y->gauche), obtenir_hauteur(y->droit)) + 1;
    return y;
}

Noeud* inserer(Noeud* noeud, char* id, double capa, double capt, double trait) {
    if (noeud == NULL) {
        return creer_noeud(id, capa, capt, trait);
    }
    int comparaison = strcmp(id, noeud->identifiant);
    
    if (comparaison < 0) {
        noeud->gauche = inserer(noeud->gauche, id, capa, capt, trait);
    } 
    else if (comparaison > 0) {
        noeud->droit = inserer(noeud->droit, id, capa, capt, trait);
    } 
    else {
        noeud->capacite_max += capa;
        noeud->volume_capte += capt;
        noeud->volume_traite += trait;
        return noeud;
    }

    noeud->hauteur = 1 + maximum(obtenir_hauteur(noeud->gauche), obtenir_hauteur(noeud->droit));

    int equilibre = calculer_equilibre(noeud);

    if (equilibre > 1 && strcmp(id, noeud->gauche->identifiant) < 0) {
        return rotation_droite(noeud);
    }

    if (equilibre < -1 && strcmp(id, noeud->droit->identifiant) > 0) {
        return rotation_gauche(noeud);
    }

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

void exporter_infixe_inverse(Noeud* racine, FILE* flux_sortie) {
    if (racine) {
        exporter_infixe_inverse(racine->droit, flux_sortie);
        fprintf(flux_sortie, "%s;%.3f;%.3f;%.3f\n", racine->identifiant, racine->capacite_max, racine->volume_capte, racine->volume_traite);
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
