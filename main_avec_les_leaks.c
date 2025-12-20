#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "leaks.h"

void afficher_menu() {
    printf("\n--- SYSTÈME DE GESTION DU RÉSEAU D'EAU ---\n");
    printf("1. Capacité maximale de traitement (Export vol_max.dat)\n");
    printf("2. Volume total capté depuis les sources (Export vol_captation.dat)\n");
    printf("3. Volume total traité par les usines (Export vol_traitement.dat)\n");
    printf("4. Calculer le rendement de distribution (Mode LEAKS)\n");
    printf("5. Quitter\n");
    printf("------------------------------------------\n");
    printf("Votre choix : ");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier_donnees.csv>\n", argv[0]);
        return 1;
    }

    FILE* fichier = fopen(argv[1], "r");
    if (!fichier) {
        perror("Erreur lors de l'ouverture du fichier source");
        return 2;
    }

    Noeud* racine = NULL;
    char ligne[1024];

    printf("Chargement de l'AVL en cours...\n");
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] == '\n' || ligne[0] == '\r') continue;

        char *c1 = strtok(ligne, ";"); 
        char *c2 = strtok(NULL, ";");  
        char *c3 = strtok(NULL, ";");  
        char *c4 = strtok(NULL, ";");  
        char *c5 = strtok(NULL, ";\n"); 

        if (c3 && strcmp(c3, "-") != 0) {
            double v4 = (c4) ? atof(c4) : 0.0;
            double v5 = (c5) ? atof(c5) : 0.0;
            double v_traite = v4 * (1.0 - (v5 / 100.0));
            // Insertion dans l'AVL (trié par identifiant)
            racine = inserer(racine, c3, v4, v4, v_traite);
        }
    }
    fclose(fichier);
    printf("Données chargées.\n");

    int choix = 0;
    char id_saisie[100];
    FILE* f_out = NULL;

    while (choix != 5) {
        afficher_menu();
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); 
            continue;
        }

        switch (choix) {
            case 1: // CAPACITÉ MAX
                f_out = fopen("vol_max.dat", "w");
                if (f_out) {
                    fprintf(f_out, "identifiant;capacite_max_Mm3\n");
                    // Fonction d'exportation (déjà configurée pour l'ordre alphabétique inverse)
                    exporter_infixe_inverse(racine, f_out); 
                    fclose(f_out);
                    printf("Fichier 'vol_max.dat' généré.\n");
                }
                break;

            case 2: // VOLUME CAPTÉ
                f_out = fopen("vol_captation.dat", "w");
                if (f_out) {
                    fprintf(f_out, "identifiant;volume_capte_Mm3\n");
                    exporter_infixe_inverse(racine, f_out); 
                    fclose(f_out);
                    printf("Fichier 'vol_captation.dat' généré.\n");
                }
                break;

            case 3: // VOLUME TRAITÉ
                f_out = fopen("vol_traitement.dat", "w");
                if (f_out) {
                    fprintf(f_out, "identifiant;volume_traite_Mm3\n");
                    exporter_infixe_inverse(racine, f_out); 
                    fclose(f_out);
                    printf("Fichier 'vol_traitement.dat' généré.\n");
                }
                break;

            case 4: // RENDEMENT / LEAKS
                printf("Entrez l'identifiant de l'usine : ");
                scanf("%s", id_saisie);
                // La fonction executer_mode_leaks gère déjà l'ID -1 et l'historique .dat
                executer_mode_leaks(racine, id_saisie);
                break;

            case 5:
                printf("Fin du programme.\n");
                break;

            default:
                printf("Choix invalide.\n");
        }
    }

    if (racine) liberer_arbre(racine);
    return 0;
}
