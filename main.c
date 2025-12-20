#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

// Déclaration de la fonction externe définie dans leaks.c
extern void executer_mode_leaks(Noeud* racine, char* target_id);

int main(int argc, char* argv[]) {
    // 1. Vérification des arguments
    if (argc < 4) {
        fprintf(stderr, "Erreur : Arguments manquants.\n");
        return 1;
    }

    FILE* fichier = fopen(argv[1], "r");
    if (!fichier) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 2;
    }

    char* mode = argv[2];
    char* option_id = argv[3];
    Noeud* racine = NULL;
    char ligne[1024];

    // 2. Lecture du fichier CSV
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\r') continue;

        char *c1 = strtok(ligne, ";"); // Usine ID (Col 1)
        char *c2 = strtok(NULL, ";");  // Amont ID (Col 2)
        char *c3 = strtok(NULL, ";");  // Aval ID (Col 3)
        char *c4 = strtok(NULL, ";");  // Volume/Capacité (Col 4)
        char *c5 = strtok(NULL, ";\n"); // % Fuite (Col 5)

        double val4 = (c4 && c4[0] != '-') ? atof(c4) : 0.0;
        double val5 = (c5 && c5[0] != '-') ? atof(c5) : 0.0;

        if (strcmp(mode, "histo") == 0) {
            // --- MODE HISTO ---
            if (c2 && strstr(c2, "Facility") && (!c3 || c3[0] == '-')) {
                racine = inserer(racine, c2, val4, 0, 0);
            }
            else if (c2 && strstr(c2, "Spring")) {
                double volume_traite = val4 * (1.0 - (val5 / 100.0));
                // Astuce : on met val4 dans la colonne max pour que l'option 'max' fonctionne
                racine = inserer(racine, c3, val4, val4, volume_traite);
            }
        } 
        else if (strcmp(mode, "leaks") == 0) {
            // --- MODE LEAKS ---
            // On stocke tous les tronçons dans l'AVL pour le calcul récursif
            if (c3 && strcmp(c3, "-") != 0) {
                double volume_traite = val4 * (1.0 - (val5 / 100.0));
                racine = inserer(racine, c3, 0, val4, volume_traite);
            }
        }
    }
    fclose(fichier);

    // 3. Exportation et Traitement final
    if (strcmp(mode, "histo") == 0) {
        // Sortie standard pour les graphiques
        char nom_sortie[128];
        sprintf(nom_sortie, "resultat_%s.dat", option_id);
        FILE* flux_out = fopen(nom_sortie, "w");
        if (flux_out) {
            fprintf(flux_out, "identifiant;volume_max;volume_capte;volume_traite\n");
            exporter_infixe_inverse(racine, flux_out);
            fclose(flux_out);
        }
    } 
    else if (strcmp(mode, "leaks") == 0) {
        // Appel de la logique métier dans leaks.c
        // Cette fonction s'occupe de calculer et d'écrire dans rendement_historique.dat
        executer_mode_leaks(racine, option_id);
    }

    // 4. Nettoyage
    liberer_arbre(racine);

    return 0;
}
