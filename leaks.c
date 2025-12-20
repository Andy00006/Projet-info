#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leaks.h"

double sommerPertesAval(Noeud* racine, char* id_actuel, double debit_entrant) {
    Noeud* n = rechercher_noeud(racine, id_actuel);
    if (!n) return 0;

    double perte_locale = 0;
    double debit_sortant = debit_entrant;

    if (debit_entrant > n->capacite_max) {
        perte_locale = debit_entrant - n->capacite_max;
        debit_sortant = n->capacite_max;
    }

    double pertes_sous_reseau = 0;
    int nb_enfants = (n->gauche != NULL) + (n->droit != NULL);

    if (nb_enfants > 0) {
        double debit_par_enfant = debit_sortant / nb_enfants;
        if (n->gauche) pertes_sous_reseau += sommerPertesAval(racine, n->gauche->identifiant, debit_par_enfant);
        if (n->droit) pertes_sous_reseau += sommerPertesAval(racine, n->droit->identifiant, debit_par_enfant);
    }
    return perte_locale + pertes_sous_reseau;
}

void executer_mode_leaks(Noeud* racine, char* target_id) {
    Noeud* usine = rechercher_noeud(racine, target_id);
    FILE* f_hist = fopen("rendement_historique.dat", "a");
    
    if (!f_hist) return;

    if (!usine) {
        fprintf(f_hist, "%s;-1.000\n", target_id);
        printf("-1\n");
    } else {
        double total_pertes = sommerPertesAval(racine, target_id, usine->capacite_max);
        fprintf(f_hist, "%s;%.3f\n", target_id, total_pertes / 1000.0);
        printf("Total des pertes pour %s : %.3f M.m3\n", target_id, total_pertes / 1000.0);
    }
    fclose(f_hist);
}
