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


