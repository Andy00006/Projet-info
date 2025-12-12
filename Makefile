#ifndef AVL_H
#define AVL_H

#include <stdio.h>

/* AVL simple clé -> pointeur de donnée (void*). */
typedef struct NoeudAVL {
    char *cle;               /* clé (identifiant) */
    void *val;               /* pointeur vers donnée (factory_t ou node_t) */
    struct NoeudAVL *gauche;
    struct NoeudAVL *droite;
    int hauteur;
} NoeudAVL;

typedef struct {
    NoeudAVL *racine;
} AVL;

/* création/libération */
AVL *avl_creer(void);
void avl_liberer(AVL *a, void (*liberer_val)(void*));

/* recherche: retourne val si trouvée, NULL sinon */
void *avl_trouver(AVL *a, const char *cle);

/* insertion si absente: insère val et retourne val.
   Si cle existe déjà, retourne la val existante et n'insère pas.
*/
void *avl_inserer_si_absent(AVL *a, const char *cle, void *val);

/* parcours reverse alphabetical (descendant) : appelle fn(val, cle, ctx) */
void avl_parcours_desc(AVL *a, void (*fn)(void *val, const char *cle, void *ctx), void *ctx);

#endif
