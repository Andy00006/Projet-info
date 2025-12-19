typedef struct Noeud {
    int id;
    double capacite;       
    int id_enfant_gauche;  
    int id_enfant_droit;  
    struct Noeud *gauche, *droite;
} Noeud;
Noeud* rechercher(Noeud* r, int id) {
    if (r == NULL || r->id == id)
    {return r;}
    if (id < r->id) 
    {return rechercher(r->gauche, id)};
    return rechercher(r->droite, id);
}
double sommerPertesAval(Noeud* racine, int id_actuel, double debit_entrant) {
    Noeud* n = rechercher(racine, id_actuel);
    if (!n) return 0;
    double perte_locale = 0;
    double debit_sortant = debit_entrant;
    if (debit_entrant > n->capacite) {
        perte_locale = debit_entrant - n->capacite;
        debit_sortant = n->capacite;
    }
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
    extern Noeud* racine; 
    int id_usine;
    printf("Entrez l'identifiant de l'usine : ");
    if (scanf("%d", &id_usine) != 1) return 1;
    Noeud* usine = rechercher(racine, id_usine);
    if (usine == NULL) {
        printf("-1\n");
    } else {
        double volumePerdu = sommerPertesAval(racine, id_usine, usine->capacite);
        printf("Total des pertes en aval : %.2f M.m3\n", volumePerdu);
        FILE *fichier = fopen("historique_pertes.dat", "a");
        if (fichier != NULL) {
            fprintf(fichier, "%d %.2f\n", id_usine, volumePerdu);
            fclose(fichier);}
    }
    return 0;
}
