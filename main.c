#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

int main(int argc, char* argv[]) {
    if (argc < 4) return 1;

    FILE* fichier = fopen(argv[1], "r");
    if (!fichier) return 2;

    char* mode = argv[2];
    char* option_id = argv[3];
    Noeud* racine = NULL;
    char ligne[1024];
    
    int usine_trouvee = 0;
    double total_fuites = 0;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n') continue;
        // NOTE: On ne saute plus les lignes commençant par '-' car tes données en ont !

        char *c1 = strtok(ligne, ";"); 
        char *c2 = strtok(NULL, ";");  
        char *c3 = strtok(NULL, ";");  
        char *c4 = strtok(NULL, ";");  
        char *c5 = strtok(NULL, ";\n"); 

        double v4 = (c4) ? atof(c4) : 0.0;
        double v5 = (c5) ? atof(c5) : 0.0;

        if (strcmp(mode, "histo") == 0) {
            // Dans tes données : c2 est la source/amont, c3 est l'usine/aval
            if (c2 && c3 && strcmp(c3, "-") != 0) {
                // On calcule le volume traité qui arrive à l'usine (c3)
                double v_traite = v4 * (1.0 - (v5 / 100.0));
                // On insère l'usine (c3) avec le volume capté (v4) et traité
                racine = inserer(racine, c3, v4, v4, v_traite);
            }
        } 
        else if (strcmp(mode, "leaks") == 0) {
            // Pour le mode leaks, on vérifie si l'ID d'usine (option_id) est en c3
            if (c3 && strcmp(c3, option_id) == 0) {
                usine_trouvee = 1;
                total_fuites += (v4 * (v5 / 100.0));
            }
        }
    }
    fclose(fichier);

    // --- SORTIE DES DONNÉES ---
    char nom_sortie[256];
    if (strcmp(mode, "histo") == 0) {
        snprintf(nom_sortie, sizeof(nom_sortie), "resultat_%s.dat", option_id);
        FILE* flux_out = fopen(nom_sortie, "w");
        if (flux_out) {
            fprintf(flux_out, "identifiant;volume_max;volume_capte;volume_traite\n");
            exporter_infixe_inverse(racine, flux_out);
            fclose(flux_out);
        }
    } else if (strcmp(mode, "leaks") == 0) {
        FILE* f_hist = fopen("rendement_historique.dat", "a");
        if (f_hist) {
            fprintf(f_hist, "%s;%.3f\n", option_id, usine_trouvee ? (total_fuites / 1000.0) : -1.0);
            fclose(f_hist);
        }
    }

    if (racine) liberer_arbre(racine);
    return 0;
}
