#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "avl.h"

// --- Libération Mémoire ---
void liberer_usine_val(void *v) {
    if (!v) return;
    usine_t *u = (usine_t *)v;
    free(u->id);
    free(u);
}

void liberer_noeud_val(void *v) {
    if (!v) return;
    noeud_graphe_t *n = (noeud_graphe_t *)v;
    free(n->id);
    Edge *e = n->enfants;
    while (e) {
        Edge *tmp = e;
        e = e->suivant;
        free(tmp);
    }
    free(n);
}

// --- Parsing ---
static double parse_double(const char *s) {
    if (!s || strcmp(s, "-") == 0 || s[0] == '\0') return 0.0;
    return atof(s);
}

static void ajouter_arc(AVL *avl_noeuds, const char *amont, const char *aval, double fuite) {
    noeud_graphe_t *n_amont = (noeud_graphe_t *)avl_trouver(avl_noeuds, amont);
    if (!n_amont) {
        n_amont = calloc(1, sizeof(noeud_graphe_t));
        n_amont->id = strdup(amont);
        avl_inserer_si_absent(avl_noeuds, amont, n_amont);
    }
    noeud_graphe_t *n_aval = (noeud_graphe_t *)avl_trouver(avl_noeuds, aval);
    if (!n_aval) {
        n_aval = calloc(1, sizeof(noeud_graphe_t));
        n_aval->id = strdup(aval);
        avl_inserer_si_absent(avl_noeuds, aval, n_aval);
    }
    Edge *e = malloc(sizeof(Edge));
    e->fils = n_aval;
    e->fuite_pct = fuite;
    e->suivant = n_amont->enfants;
    n_amont->enfants = e;
}

// --- Logique CSV ---
int construire_depuis_csv(const char *chemin, AVL *avl_usines, AVL *avl_noeuds) {
    FILE *f = fopen(chemin, "r");
    if (!f) return 1;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        char *tmp = strdup(line);
        char *cols[5];
        int i = 0;
        char *token = strtok(tmp, ";");
        while (i < 5) {
            cols[i++] = token ? token : "-";
            token = strtok(NULL, ";");
        }

        // 1. Usine seule
        if (strcmp(cols[1], "-") != 0 && strcmp(cols[2], "-") == 0) {
            usine_t *u = avl_trouver(avl_usines, cols[1]);
            if (!u) {
                u = calloc(1, sizeof(usine_t));
                u->id = strdup(cols[1]);
                avl_inserer_si_absent(avl_usines, cols[1], u);
            }
            u->capacite_max_km3 = parse_double(cols[3]);
        }
        // 2. Source -> Usine
        else if (strncmp(cols[1], "Spring", 6) == 0) {
            usine_t *u = avl_trouver(avl_usines, cols[2]);
            if (!u) {
                u = calloc(1, sizeof(usine_t));
                u->id = strdup(cols[2]);
                avl_inserer_si_absent(avl_usines, cols[2], u);
            }
            double cap = parse_double(cols[3]);
            u->total_src_km3 += cap;
            u->total_real_km3 += cap * (1.0 - parse_double(cols[4])/100.0);
        }
        // 3. Graphe aval
        if (strcmp(cols[1], "-") != 0 && strcmp(cols[2], "-") != 0) {
            ajouter_arc(avl_noeuds, cols[1], cols[2], parse_double(cols[4]));
        }
        free(tmp);
    }
    fclose(f);
    return 0;
}

// --- Histogramme ---
static void ecrire_ligne(void *val, const char *cle, void *ctx) {
    (void)cle; usine_t *u = (usine_t*)val; FILE *f = (FILE*)ctx;
    fprintf(f, "%s,%.6f,%.6f,%.6f\n", u->id, u->capacite_max_km3/1000.0, u->total_src_km3/1000.0, u->total_real_km3/1000.0);
}

int generer_histo(AVL *avl_usines, const char *mode) {
    char name[64]; snprintf(name, 64, "vol_%s.dat", mode);
    FILE *f = fopen(name, "w");
    fprintf(f, "identifier,max,src,real\n");
    avl_parcours_desc(avl_usines, ecrire_ligne, f);
    fclose(f);
    return 0;
}

// --- Leaks ---
static double dfs_leak(noeud_graphe_t *n, double flow) {
    if (!n || !n->enfants) return 0.0;
    int count = 0; for (Edge *e = n->enfants; e; e = e->suivant) count++;
    double per_child = flow / count;
    double total = 0.0;
    for (Edge *e = n->enfants; e; e = e->suivant) {
        double leak = per_child * (e->fuite_pct / 100.0);
        total += leak + dfs_leak(e->fils, per_child - leak);
    }
    return total;
}

double calculer_fuites(AVL *avl_usines, AVL *avl_noeuds, const char *id) {
    usine_t *u = avl_trouver(avl_usines, id);
    if (!u) return -1.0;
    noeud_graphe_t *n = avl_trouver(avl_noeuds, id);
    double res = n ? dfs_leak(n, u->total_real_km3) / 1000.0 : 0.0;
    FILE *h = fopen("leaks_history.dat", "a");
    if (h) { fprintf(h, "%s;%.6f\n", id, res); fclose(h); }
    return res;
}
