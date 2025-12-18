#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "data.h"

int main(int argc, char **argv) {
    if (argc < 4) return 1;

    const char *action = argv[1];
    const char *csv = argv[2];
    const char *param = argv[3];

    AVL *avl_usines = avl_creer();
    AVL *avl_noeuds = avl_creer();

    if (construire_depuis_csv(csv, avl_usines, avl_noeuds) != 0) {
        avl_liberer(avl_usines, liberer_usine_val);
        avl_liberer(avl_noeuds, liberer_noeud_val);
        return 2;
    }

    if (strcmp(action, "histo") == 0) {
        generer_histo(avl_usines, param);
    } else if (strcmp(action, "leaks") == 0) {
        double res = calculer_fuites(avl_usines, avl_noeuds, param);
        if (res < 0) printf("-1\n");
        else printf("%.6f\n", res);
    }

    avl_liberer(avl_usines, liberer_usine_val);
    avl_liberer(avl_noeuds, liberer_noeud_val);
    return 0;
}
