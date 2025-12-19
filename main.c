#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

int main(int argc, char* argv[]) {
    // Vérification stricte des arguments (Fichier, Mode, Option/ID)
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

    // Lecture du fichier CSV
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // On saute les lignes vides ou celles qui commencent par un tiret (si nécessaire)
        if (ligne[0] == '\n') continue;

        // Découpage de la ligne avec strtok
        char *c1 = strtok(ligne, ";"); // Usine ID (Col 1)
        char *c2 = strtok(NULL, ";");  // Amont ID (Col 2)
        char *c3 = strtok(NULL, ";");  // Aval ID (Col 3)
        char *c4 = strtok(NULL, ";");  // Volume/Capacité (Col 4)
        char *c5 = strtok(NULL, ";\n"); // % Fuite (Col 5)

        // Conversion sécurisée des valeurs numériques
        double val4 = (c4 && c4[0] != '-') ? atof(c4) : 0.0;
        double val5 = (c5 && c5[0] != '-') ? atof(c5) : 0.0;

        if (strcmp(mode, "histo") == 0) {
            // --- MODE HISTO ---
            // Cas Usine : Info capacité (Col 2 = ID Usine, Col 4 = Capacité)
            if (c2 && strstr(c2, "Facility") && (!c3 || c3[0] == '-')) {
                racine = inserer(racine, c2, val4, 0, 0);
            }
            // Cas Source -> Usine : Captage (Col 2 = Source, Col 3 = Usine)
            else if (c2 && strstr(c2, "Spring")) {
                double volume_traite = val4 * (1.0 - (val5 / 100.0));
                racine = inserer(racine, c3, 0, val4, volume_traite);
            }
        } 
        else if (strcmp(mode, "leaks") == 0) {
            // --- MODE LEAKS ---
            // On utilise enfin c1 pour filtrer les tronçons appartenant à l'usine choisie
            if (c1 && strcmp(c1, option_id) == 0) {
                // Pour le moment, on stocke l'ID dans l'AVL pour confirmer qu'on le trouve.
                // Le calcul réel des fuites cumulées nécessitera l'arbre de distribution.
                racine = inserer(racine, c1, 0, 0, 0);
            }
        }
    }

    // Exportation des résultats
    char nom_sortie[128];
    if (strcmp(mode, "histo") == 0) {
        sprintf(nom_sortie, "resultat_%s.dat", option_id);
    } else {
        sprintf(nom_sortie, "fuites_usine.dat");
    }

    FILE* flux_out = fopen(nom_sortie, "w");
    if (flux_out) {
        if (strcmp(mode, "histo") == 0) {
            fprintf(flux_out, "identifiant;volume_max;volume_capte;volume_traite\n");
        } else {
            fprintf(flux_out, "identifiant;volume_fuites_cumulees\n");
        }
        exporter_infixe_inverse(racine, flux_out);
        fclose(flux_out);
    }

    // Nettoyage final
    fclose(fichier);
    liberer_arbre(racine);

    return 0;
}
