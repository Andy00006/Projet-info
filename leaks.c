#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

Noeud* rechercher_usine(Noeud* r, char* id) {
    if (r == NULL) {
        return NULL;
    }
    int comp = strcmp(id, r->identifiant);
    if (comp == 0) {
        return r;
    }
    if (comp < 0) {
        return rechercher_usine(r->gauche, id);
    } 
    else {
        return rechercher_usine(r->droit, id);
    }
}

double calculer_pertes(Noeud* racine, char* id_actuel) {
    Noeud* n = rechercher_usine(racine, id_actuel);
    if (n == NULL) {
        return 0.0;
    }
    double perte = n->volume_capte - n->volume_traite;
    return perte;
}

void executer_mode_leaks(Noeud* racine, char* target_id) {
    FILE* f_hist = fopen("rendement_historique.dat", "a");
    if (f_hist == NULL) {
        return;
    }
    Noeud* usine = rechercher_usine(racine, target_id);
    if (usine == NULL) {
        fprintf(f_hist, "%s;-1.000\n", target_id);
        printf("-1\n");
    } 
    else {
        double total_pertes = usine->volume_capte - usine->volume_traite;
        fprintf(f_hist, "%s;%.3f\n", target_id, total_pertes);
        printf("Pertes pour %s : %.3f M.m3\n", target_id, total_pertes);
    }
    fclose(f_hist);
}
