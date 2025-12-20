#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "leaks.h"

int main(int argc, char* argv[]) {
    // 1. Vérification des arguments (Fichier, Mode, ID)
    if (argc < 4) {
        fprintf(stderr, "Erreur : Arguments insuffisants.\n");
        fprintf(stderr, "Usage: %s <fichier.csv> <histo|leaks> <id_station>\n", argv[0]);
        return 1;
    }

    // 2. Ouverture du fichier source
    FILE* fichier = fopen(argv[1], "r");
    if (!fichier) {
        perror("Erreur lors de l'ouverture du fichier");
        return 2;
    }

    char* mode = argv[2];
    char* option_id = argv[3];
    Noeud* racine = NULL;
    char ligne[1024];

    // 3. Chargement systématique de l'AVL pour construire la structure du réseau
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\r') continue;

        // Découpage de la ligne CSV (format: col1;col2;ID;Capacité;Rendement)
        char *c1 = strtok(ligne, ";"); 
        char *c2 = strtok(NULL, ";");  
        char *c3 = strtok(NULL, ";");  // Identifiant de la station
        char *c4 = strtok(NULL, ";");  // Volume ou Capacité
        char *c5 = strtok(NULL, ";\n"); // Pourcentage de fuite ou rendement

        if (c3 && strcmp(c3, "-") != 0) {
            double v4 = (c4) ? atof(c4) : 0.0;
            double v5 = (c5) ? atof(c5) : 0.0;
            
            // Calcul du volume traité pour le mode histo
            double v_traite = v4 * (1.0 - (v5 / 100.0));

            // Insertion dans l'arbre (défini dans avl.c)
            racine = inserer(racine, c3, v4, v4, v_traite);
        }
    }
    fclose(fichier);

    // 4. Aiguillage selon le mode choisi
    if (strcmp(mode, "histo") == 0) {
        char nom_sortie[256];
        snprintf(nom_sortie, sizeof(nom_sortie), "resultat_%s.dat", option_id);
        
        FILE* flux_out = fopen(nom_sortie, "w");
        if (flux_out) {
            fprintf(flux_out, "identifiant;volume_max;volume_capte;volume_traite\n");
            exporter_infixe_inverse(racine, flux_out);
            fclose(flux_out);
        } else {
            perror("Erreur lors de la création du fichier de sortie");
        }
    } 
    else if (strcmp(mode, "leaks") == 0) {
        // Appel de la fonction modulaire (définie dans leaks.c)
        executer_mode_leaks(racine, option_id);
    } 
    else {
        fprintf(stderr, "Erreur : Mode '%s' inconnu (utilisez 'histo' ou 'leaks').\n", mode);
    }

    // 5. Nettoyage de la mémoire
    if (racine) {
        liberer_arbre(racine);
    }

    return 0;
}
