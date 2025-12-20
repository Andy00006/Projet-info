#!/bin/bash

# 1. Capture du temps de début (en millisecondes)
debut=$(date +%s%3N)

# --- FONCTIONS ---

afficher_usage() {
    echo "Usage: $0 <fichier_donnees> <mode> <option/ID>"
    echo "Modes disponibles :"
    echo "  histo <max|src|real>"
    echo "  leaks <ID_Usine>"
    exit 1
}

# --- VÉRIFICATIONS DES ARGUMENTS ---

if [ "$#" -lt 3 ]; then
    afficher_usage
fi

FICHIER_CSV=$1
MODE=$2
OPTION=$3

# Vérification rigoureuse de l'option pour le mode histo
if [ "$MODE" == "histo" ]; then
    if [[ "$OPTION" != "max" && "$OPTION" != "src" && "$OPTION" != "real" ]]; then
        echo "Erreur : L'option '$OPTION' est invalide."
        echo "Options acceptées pour 'histo' : max, src, real"
        exit 1
    fi
fi

if [ ! -f "$FICHIER_CSV" ]; then
    echo "Erreur : Le fichier '$FICHIER_CSV' est introuvable."
    exit 2
fi

# --- COMPILATION ---

if [ ! -f "./water_processor" ]; then
    echo "Exécutable introuvable. Compilation..."
    make
    [ $? -ne 0 ] && echo "Erreur de compilation." && exit 3
fi

# --- EXÉCUTION DU PROGRAMME C ---

echo "Traitement en cours... (veuillez patienter)"
./water_processor "$FICHIER_CSV" "$MODE" "$OPTION"
CODE_RETOUR=$?

if [ $CODE_RETOUR -ne 0 ]; then
    echo "Le programme C a rencontré une erreur (Code: $CODE_RETOUR)."
else
    echo "Traitement C terminé avec succès."
fi

# --- GÉNÉRATION DES GRAPHIQUES ---

if [ "$MODE" == "histo" ] && [ $CODE_RETOUR -eq 0 ]; then
    FICHIER_DAT="resultat_${OPTION}.dat"
    
    # Détermination de la colonne
    COL=2
    [ "$OPTION" == "src" ] && COL=3
    [ "$OPTION" == "real" ] && COL=4

    echo "Pré-tri des données pour Gnuplot..."
    
    # --- LA SOLUTION AU BROKEN PIPE : TRI EXTERNE ---
    # Top 10 décroissant
    sort -t';' -k${COL} -rn "$FICHIER_DAT" | head -n 10 > top10.tmp
    # Bottom 50 croissant (exclut l'en-tête et les 0)
    sort -t';' -k${COL} -n "$FICHIER_DAT" | awk -F';' -v c="$COL" '$c > 0' | head -n 50 > bot50.tmp

    gnuplot << EOF
        set datafile separator ";"
        set terminal png size 1000,700
        set style data histograms
        set style fill solid 1.0
        set xtics rotate by -45 font ",8"
        set grid y
        set ylabel "Volume (M.m3)"
        
        set output "${OPTION}_top10.png"
        set title "Top 10 - ${OPTION}"
        plot "top10.tmp" using ${COL}:xtic(1) title "Volume"
        
        set output "${OPTION}_bot50.png"
        set title "50 plus petites - ${OPTION}"
        plot "bot50.tmp" using ${COL}:xtic(1) title "Volume"
EOF
    
    rm -f top10.tmp bot50.tmp
    echo "Graphiques générés : ${OPTION}_top10.png et ${OPTION}_bot50.png"
fi

# --- FIN ET CHRONOMÈTRE ---

fin=$(date +%s%3N)
duree=$((fin - debut))

echo "-------------------------------------------"
echo "Durée totale du script : $duree ms"
echo "-------------------------------------------"
