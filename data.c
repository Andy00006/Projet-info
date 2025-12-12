
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

/* helpers */
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *r = malloc(n + 1);
    if (!r) return NULL;
    memcpy(r, s, n + 1);
    return r;
}

/* free usine (CORRECTION: n'est plus static et nom adapté) */
void liberer_usine_val(void *v) {
    if (!v) return;
    usine_t *u = (usine_t*)v;
    free(u->id);
    free(u);
}

/* free noeud graphe (CORRECTION: n'est plus static et nom adapté) */
void liberer_noeud_val(void *v) {
    if (!v) return;
    noeud_graphe_t *n = (noeud_graphe_t*)v;
    Edge *e = n->enfants;
    while (e) {
        Edge *nx = e->suivant;
        free(e);
        e = nx;
    }
    free(n->id);
    free(n);
}

/* parse une ligne en 5 colonnes séparées par ';' */
/* retourne 1 si lu, 0 EOF */
static int lire_ligne(FILE *f, char **c1, char **c2, char **c3, char **c4, char **c5) {
    char *ligne = NULL;
    size_t len = 0;
    ssize_t r = getline(&ligne, &len, f);
    if (r == -1) {
        free(ligne);
        return 0;
    }
    /* découpe manuel pour tolérer champs manquants */
    char *p = ligne;
    char *parts[5] = {NULL,NULL,NULL,NULL,NULL};
    int idx = 0;
    char *start = p;
    while (*p && idx < 4) {
        if (*p == ';') {
            *p = '\0';
            parts[idx++] = start;
            start = p + 1;
        }
        p++;
    }
    /* dernier champ */
    parts[idx++] = start;
    /* trim newline on last */
    for (int i = 0; i < 5; ++i) {
        if (!parts[i]) {
            parts[i] = "";
        } else {
            /* replace trailing newline */
            char *nl = strchr(parts[i], '\n');
            if (nl) *nl = '\0';
            char *cr = strchr(parts[i], '\r');
            if (cr) *cr = '\0';
        }
    }
    *c1 = xstrdup(parts[0]);
    *c2 = xstrdup(parts[1]);
    *c3 = xstrdup(parts[2]);
    *c4 = xstrdup(parts[3]);
    *c5 = xstrdup(parts[4]);
    free(ligne);
    return 1;
}

static double parse_double(const char *s) {
    if (!s) return 0.0;
    while (*s == ' ' || *s == '\t') s++;
    if (s[0] == '-' || s[0] == '\0') return 0.0;
    char *end;
    double v = strtod(s, &end);
    if (end == s) return 0.0;
    return v;
}

/* ajout edge parent->child (si noeuds absents on les crée) */
static void ajouter_arc(AVL *avl_noeuds, const char *parent, const char *child, double fuite_pct) {
    if (!parent || !child) return;
    noeud_graphe_t *np = (noeud_graphe_t*)avl_trouver(avl_noeuds, parent);
    if (!np) {
        np = malloc(sizeof(noeud_graphe_t));
        np->id = xstrdup(parent);
        np->enfants = NULL;
        avl_inserer_si_absent(avl_noeuds, parent, np);
    }
    noeud_graphe_t *nc = (noeud_graphe_t*)avl_trouver(avl_noeuds, child);
    if (!nc) {
        nc = malloc(sizeof(noeud_graphe_t));
        nc->id = xstrdup(child);
        nc->enfants = NULL;
        avl_inserer_si_absent(avl_noeuds, child, nc);
    }
    /* créer edge */
    Edge *e = malloc(sizeof(Edge));
    e->fils = nc;
    e->fuite_pct = fuite_pct;
    e->suivant = np->enfants;
    np->enfants = e;
}

/* construit AVL usines et AVL noeuds (graphe) à partir du CSV */
/* retourne 0 succès, >0 erreur */
int construire_depuis_csv(const char *chemin, AVL *avl_usines, AVL *avl_noeuds) {
    FILE *f = fopen(chemin, "r");
    if (!f) {
        perror("open csv");
        return 1;
    }
    char *c1,*c2,*c3,*c4,*c5;
    while (lire_ligne(f, &c1,&c2,&c3,&c4,&c5)) {
        /* DETECTION: ligne usine: col2 contient "Facility complex" et col3 vide ou "-" */
        if (strstr(c2, "Facility complex") && (c3[0]=='\0' || c3[0]=='-' )) {
            usine_t *u = malloc(sizeof(usine_t));
            u->id = xstrdup(c2);
            u->capacite_max_km3 = parse_double(c4);
            u->total_src_km3 = 0.0;
            u->total_real_km3 = 0.0;
            avl_inserer_si_absent(avl_usines, u->id, u);
        }
        /* source -> usine : col3 contient Facility complex */
        if (strstr(c3, "Facility complex")) {
            usine_t *u = (usine_t*)avl_trouver(avl_usines, c3);
            if (!u) {
                u = malloc(sizeof(usine_t));
                u->id = xstrdup(c3);
                u->capacite_max_km3 = 0.0;
                u->total_src_km3 = 0.0;
                u->total_real_km3 = 0.0;
                avl_inserer_si_absent(avl_usines, u->id, u);
            }
            double captured = parse_double(c4);
            double fuite_pct = parse_double(c5);
            u->total_src_km3 += captured;
            u->total_real_km3 += captured * (1.0 - fuite_pct/100.0);
        }
        /* si col2 et col3 présents -> arc parent->child (pour graphe) */
        if (c2[0] != '\0' && c2[0] != '-' && c3[0] != '\0' && c3[0] != '-') {
            double fuite_pct = parse_double(c5);
            ajouter_arc(avl_noeuds, c2, c3, fuite_pct);
        }
        free(c1); free(c2); free(c3); free(c4); free(c5);
    }
    fclose(f);
    return 0;
}

/* écriture fichier vol_<mode>.dat, header obligatoire */
typedef struct { FILE *f; const char *mode; } ctx_histo_t;

static void ecrire_ligne_histo(void *val, const char *cle, void *ctx) {
    (void)cle;
    ctx_histo_t *c = (ctx_histo_t*)ctx;
    usine_t *u = (usine_t*)val;
    if (!u) return;
    if (strcmp(c->mode, "max")==0) {
        fprintf(c->f, "%s,%.6f,%.6f,%.6f\n", u->id, u->capacite_max_km3, u->total_src_km3, u->total_real_km3);
    } else if (strcmp(c->mode, "src")==0) {
        fprintf(c->f, "%s,%.6f,%.6f,%.6f\n", u->id, u->capacite_max_km3, u->total_src_km3, u->total_real_km3);
    } else { /* real */
        fprintf(c->f, "%s,%.6f,%.6f,%.6f\n", u->id, u->capacite_max_km3, u->total_src_km3, u->total_real_km3);
    }
}

int generer_histo(AVL *avl_usines, const char *mode) {
    char fname[256];
    snprintf(fname, sizeof(fname), "vol_%s.dat", mode);
    FILE *out = fopen(fname, "w");
    if (!out) { perror("open out"); return 1; }
    fprintf(out, "identifier,max volume (k.m3.year-1),source volume (k.m3.year-1),real volume (k.m3.year-1)\n");
    ctx_histo_t ctx = { .f = out, .mode = mode };
    avl_parcours_desc(avl_usines, ecrire_ligne_histo, &ctx);
    fclose(out);
    return 0;
}

/* DFS calcul fuites : flow en k.m3 */
static double dfs_leak(noeud_graphe_t *n, double flow_km3) {
    if (!n) return 0.0;
    /* compter enfants */
    int deg = 0;
    for (Edge *e = n->enfants; e; e = e->suivant) deg++;
    if (deg == 0) return 0.0;
    double per_child = flow_km3 / (double)deg;
    double total_leak = 0.0;
    for (Edge *e = n->enfants; e; e = e->suivant) {
        double leak = per_child * (e->fuite_pct / 100.0);
        total_leak += leak;
        double child_flow = per_child - leak;
        total_leak += dfs_leak(e->fils, child_flow);
    }
    return total_leak;
}

/* retourne fuites en M.m3 (millions m3) ou -1.0 si usine inconnue */
double calculer_fuites(AVL *avl_usines, AVL *avl_noeuds, const char *id_usine) {
    usine_t *u = (usine_t*)avl_trouver(avl_usines, id_usine);
    if (!u) return -1.0;
    double initial_km3 = u->total_real_km3; /* k.m3 */
    /* trouver noeud graphe correspondant à l'usine (clé identique) */
    noeud_graphe_t *n = (noeud_graphe_t*)avl_trouver(avl_noeuds, id_usine);
    if (!n) {
        /* si pas de noeud dans graphe -> pas d'aval -> fuites 0 */
        double mm3 = 0.0;
        /* écrire historique */
        FILE *hist = fopen("leaks_history.dat", "a");
        if (hist) { fprintf(hist, "%s;%.6f\n", id_usine, mm3); fclose(hist); }
        return mm3;
    }
    double leaked_km3 = dfs_leak(n, initial_km3);
    double leaked_Mm3 = leaked_km3 / 1000.0;
    /* historique */
    FILE *hist = fopen("leaks_history.dat", "a");
    if (hist) { fprintf(hist, "%s;%.6f\n", id_usine, leaked_Mm3); fclose(hist); }
    return leaked_Mm3;
}
