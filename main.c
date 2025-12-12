#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "data.h"

/* Programme unique pour histo et leaks:
   usage:
   ./eau_analyse histo <csv> <max|src|real>
   ./eau_analyse leaks <csv> "<FactoryID>"
*/

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s histo <csv> <max|src|real>\n"
            "  %s leaks <csv> \"FactoryID\"\n",
            argv[0], argv[0]);
        return 1;
    }

    /* ----------------------------------------- */
    /* MODE HISTOGRAMME                          */
    /* ----------------------------------------- */
    if (strcmp(argv[1], "histo") == 0) {
        if (argc != 4) {
            fprintf(stderr, "histo requiert: <csv> <max|src|real>\n");
            return 2;
        }

        const char *csv = argv[2];
        const char *mode = argv[3];

        if (!(strcmp(mode, "max") == 0 ||
              strcmp(mode, "src") == 0 ||
              strcmp(mode, "real") == 0)) {
            fprintf(stderr, "mode inconnu %s\n", mode);
            return 3;
        }

        AVL *avl_usines = avl_creer();
        AVL *avl_noeuds = avl_creer();

        if (construire_depuis_csv(csv, avl_usines, avl_noeuds) != 0) {
            /* Libération avec les fonctions appropriées */
            avl_liberer(avl_usines, liberer_usine_val); 
            avl_liberer(avl_noeuds, liberer_noeud_val);
            return 4;
        }

        if (generer_histo(avl_usines, mode) != 0) {
            fprintf(stderr, "Erreur lors de la génération de l'histogramme.\n");
        }

        /* libération COMPLÈTE (CORRECTION) */
        avl_liberer(avl_usines, liberer_usine_val);
        avl_liberer(avl_noeuds, liberer_noeud_val);

        printf("Fichier vol_%s.dat créé\n", mode);
        return 0;
    }

    /* ----------------------------------------- */
    /* MODE LEAKS                                */
    /* ----------------------------------------- */
    else if (strcmp(argv[1], "leaks") == 0) {
        if (argc != 4) {
            fprintf(stderr, "leaks requiert: <csv> \"FactoryID\"\n");
            return 5;
        }

        const char *csv = argv[2];
        const char *id = argv[3];

        AVL *avl_usines = avl_creer();
        AVL *avl_noeuds = avl_creer();

        if (construire_depuis_csv(csv, avl_usines, avl_noeuds) != 0) {
            /* Libération avec les fonctions appropriées */
            avl_liberer(avl_usines, liberer_usine_val);
            avl_liberer(avl_noeuds, liberer_noeud_val);
            return 6;
        }

        double res = calculer_fuites(avl_usines, avl_noeuds, id);

        if (res < 0.0)
            printf("-1\n");
        else
            printf("%.6f\n", res);

        /* libération COMPLÈTE (CORRECTION) */
        avl_liberer(avl_usines, liberer_usine_val);
        avl_liberer(avl_noeuds, liberer_noeud_val);

        return 0;
    }

    /* ----------------------------------------- */
    /* COMMANDE INCONNUE                         */
    /* ----------------------------------------- */
    else {
        fprintf(stderr, "Commande inconnue : %s\n", argv[1]);
        return 7;
    }
}
