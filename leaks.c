typedef struct Noeud {
    int id;
    double capacite;       // Volume d'eau maximal supporté/traité
    int id_enfant_gauche;  // ID du premier successeur (-1 si aucun)
    int id_enfant_droit;   // ID du second successeur (-1 si aucun)
    struct Noeud *gauche, *droite;
} Noeud;
Noeud* rechercher(Noeud* r, int id) {
    if (r == NULL || r->id == id)
    {return r;}
    if (id < r->id) {return rechercher(r->gauche, id)};
    return rechercher(r->droite, id);
}
/**
 * Calcule la somme des pertes dans tout le réseau aval
 * @param racine : pointeur vers la racine de l'AVL
 * @param id_actuel : ID du nœud en cours de traitement
 * @param debit_entrant : Volume arrivant au nœud
 */
double sommerPertesAval(Noeud* racine, int id_actuel, double debit_entrant) {
    Noeud* n = rechercher(racine, id_actuel);
    if (!n) return 0;
    // Calcul de la perte locale sur ce noeud
    // Si le débit entrant dépasse la capacité, le surplus est considéré comme perdu
    double perte_locale = 0;
    double debit_sortant = debit_entrant;
    if (debit_entrant > n->capacite) {
        perte_locale = debit_entrant - n->capacite;
        debit_sortant = n->capacite;
    }
    // Répartition équitable du débit sortant vers les enfants
    double pertes_sous_reseau = 0;
    int nb_enfants = (n->id_enfant_gauche != -1) + (n->id_enfant_droit != -1);
    if (nb_enfants > 0) {
        double debit_par_enfant = debit_sortant / nb_enfants;
        if (n->id_enfant_gauche != -1) {
            pertes_sous_reseau += sommerPertesAval(racine, n->id_enfant_gauche, debit_par_enfant);
        }
        if (n->id_enfant_droit != -1) {
            pertes_sous_reseau += sommerPertesAval(racine, n->id_enfant_droit, debit_par_enfant);
        }
    }
    return perte_locale + pertes_sous_reseau;
}
int main() {
    extern Noeud* racine; // Récupération de l'AVL déjà créé
    int id_usine;
    printf("Entrez l'identifiant de l'usine : ");
    if (scanf("%d", &id_usine) != 1) return 1;
    // Vérification de l'existence de l'usine
    Noeud* usine = rechercher(racine, id_usine);
    if (usine == NULL) {
        printf("-1\n");
    } else {
        // On initialise le calcul avec la capacité propre de l'usine
        double volumePerdu = sommerPertesAval(racine, id_usine, usine->capacite);
        // Affichage du résultat en M.m3
        printf("Total des pertes en aval : %.2f M.m3\n", volumePerdu);
        // Mise à jour ou création du fichier historique .dat
        FILE *fichier = fopen("historique_pertes.dat", "a");
        if (fichier != NULL) {
            fprintf(fichier, "%d %.2f\n", id_usine, volumePerdu);
            fclose(fichier);
        }
    }
    return 0;
}
