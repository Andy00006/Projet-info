# PROJET

Pour la bibliothèque, il faut qu'elle comporte:
<stdio.h>
<stdlib.h>
<string.h>


Pour compiler le programme,ecrire dans le terminal: make 

Pour faire fonctionner correctement le programme, il faut écrire dans le terminal la commande:
-pour avoir l'histogramme du volume maximale de traitement de l’usine : 
./myScript.sh <nom_du_fichier.dat> histo max
-pour avoir l'histogramme du volume total capté par les sources  : 
./myScript.sh <nom_du_fichier.dat> histo src
-pour avoir l'histogramme du volume total réellement traité : 
./myScript.sh <nom_du_fichier.dat> histo real
-pour creer/compléter le fichier qui mesure les pertes des usines sur l’ensemble de son réseau aval:
./myScript.sh <nom_du_fichier.dat> leaks "<nom_de_l'usine>"
-pour effacer tout les dossiers en relation avec le make: 
make clean

Les noms des fichiers créer pour les histogrammes et le fichier leaks sont:
-max_top10.png et max_bot50.png pour l'histogramme du volume maximale de traitement de l’usine
-src_top10.png et src_bot50.png pour l'histogramme du volume total capté par les sources
-real_top10.png et real_bot50.png pour l'histogramme du volume total réellement traité
-rendement_historique pour le fichier qui mesure les pertes des usines sur l’ensemble de son réseau aval

si un message d'erreur apparait, il faut réecrire la commande sans erreur pour continuer ou ecrire le message pour supprimer les fichier.
