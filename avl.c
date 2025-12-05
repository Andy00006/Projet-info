#include <stdio.h>
#include <stdlib.h>

typedef struct  abr{
	int valeur;
	abr* fg;
	abr* fd;
	int eq;
}ABR;

typedef ABR *abr;

abr creeabr(int v);
	abr new;
	new=malloc(sizeof(ABR));
	if new==NULL{
		exit(1);
	}
	new->valeur=v;
	new->fg=NULL;
	new->fd=NULL;
	new->eq=0;
}

AVL* rotationGauche(AVL* a){
    AVL* pivot = a->fd; 
    int eq_a = a->eq, eq_p = pivot->eq;

    a->fd = pivot->fg; 
    pivot->fg = a;     

    a->eq = eq_a - max(eq_p, 0) - 1;
    pivot->eq = min3(eq_a - 2, eq_a + eq_p - 2, eq_p - 1);

    return pivot; 
}

AVL* rotationDroite(AVL* a){
    AVL* pivot = a->fg; 
    int eq_a = a->eq, eq_p = pivot->eq;

    a->fg = pivot->fd; 
    pivot->fd = a;    

    a->eq = eq_a - min(eq_p, 0) + 1;
    pivot->eq = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);

    return pivot;
}

AVL* doubleRotationGauche(AVL* a){
    a->fd = rotationDroite(a->fd);
    return rotationGauche(a);
}

AVL* doubleRotationDroite(AVL* a){
    a->fg = rotationGauche(a->fg);
    return rotationDroite(a);
}
