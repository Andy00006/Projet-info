#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

static int hauteur(NoeudAVL *n) { return n ? n->hauteur : 0; }
static int max(int a, int b) { return a > b ? a : b; }

static NoeudAVL *rotate_right(NoeudAVL *y) {
    NoeudAVL *x = y->gauche;
    NoeudAVL *T2 = x->droite;
    x->droite = y;
    y->gauche = T2;
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droite)) + 1;
    x->hauteur = max(hauteur(x->gauche), hauteur(x->droite)) + 1;
    return x;
}

static NoeudAVL *rotate_left(NoeudAVL *x) {
    NoeudAVL *y = x->droite;
    NoeudAVL *T2 = y->gauche;
    y->gauche = x;
    x->droite = T2;
    x->hauteur = max(hauteur(x->gauche), hauteur(x->droite)) + 1;
    y->hauteur = max(hauteur(y->gauche), hauteur(y->droite)) + 1;
    return y;
}

static NoeudAVL *insert_node(NoeudAVL *node, const char *cle, void *val, void **existing) {
    if (!node) {
        NoeudAVL *n = calloc(1, sizeof(NoeudAVL));
        if (!n) return NULL;
        n->cle = strdup(cle);
        n->val = val;
        n->hauteur = 1;
        if (existing) *existing = NULL;
        return n;
    }
    int cmp = strcmp(cle, node->cle);
    if (cmp < 0) node->gauche = insert_node(node->gauche, cle, val, existing);
    else if (cmp > 0) node->droite = insert_node(node->droite, cle, val, existing);
    else {
        if (existing) *existing = node->val;
        return node;
    }
    node->hauteur = 1 + max(hauteur(node->gauche), hauteur(node->droite));
    int b = hauteur(node->gauche) - hauteur(node->droite);
    if (b > 1 && strcmp(cle, node->gauche->cle) < 0) return rotate_right(node);
    if (b < -1 && strcmp(cle, node->droite->cle) > 0) return rotate_left(node);
    if (b > 1 && strcmp(cle, node->gauche->cle) > 0) {
        node->gauche = rotate_left(node->gauche);
        return rotate_right(node);
    }
    if (b < -1 && strcmp(cle, node->droite->cle) < 0) {
        node->droite = rotate_right(node->droite);
        return rotate_left(node);
    }
    return node;
}

AVL *avl_creer(void) { return calloc(1, sizeof(AVL)); }

void avl_liberer_nodes(NoeudAVL *n, void (*liberer_val)(void*)) {
    if (!n) return;
    avl_liberer_nodes(n->gauche, liberer_val);
    avl_liberer_nodes(n->droite, liberer_val);
    if (liberer_val && n->val) liberer_val(n->val);
    free(n->cle);
    free(n);
}

void avl_liberer(AVL *a, void (*lib_v)(void*)) {
    if (!a) return;
    avl_liberer_nodes(a->racine, lib_v);
    free(a);
}

void *avl_trouver(AVL *a, const char *cle) {
    NoeudAVL *curr = a->racine;
    while (curr) {
        int cmp = strcmp(cle, curr->cle);
        if (cmp == 0) return curr->val;
        curr = (cmp < 0) ? curr->gauche : curr->droite;
    }
    return NULL;
}

void *avl_inserer_si_absent(AVL *a, const char *cle, void *val) {
    void *ex = NULL;
    a->racine = insert_node(a->racine, cle, val, &ex);
    return ex ? ex : val;
}

static void parcours_rev(NoeudAVL *n, void (*fn)(void*, const char*, void*), void *ctx) {
    if (!n) return;
    parcours_rev(n->droite, fn, ctx); // Droite d'abord pour l'ordre inverse
    fn(n->val, n->cle, ctx);
    parcours_rev(n->gauche, fn, ctx);
}

void avl_parcours_desc(AVL *a, void (*fn)(void*, const char*, void*), void *ctx) {
    if (a) parcours_rev(a->racine, fn, ctx);
}
    parcours_rev(a->racine, fn, ctx);
}
