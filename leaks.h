#ifndef LEAKS_H
#define LEAKS_H

#include "avl.h"

// Calcule le volume total perdu en aval d'une station
double sommerPertesAval(Noeud* racine, char* id_actuel, double debit_entrant);

// Gère l'exécution du mode leaks (recherche, calcul et écriture fichier)
void executer_mode_leaks(Noeud* racine, char* target_id);

#endif
