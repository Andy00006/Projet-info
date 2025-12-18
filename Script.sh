#!/usr/bin/env bash
set -euo pipefail

# Définition des codes d'erreur
ERR_ARGS=1
ERR_CSV=2
ERR_COMPIL=3
ERR_H_MODE=5
ERR_C_FAIL=7
ERR_ACTION=8

USAGE="Usage: $0 <fichier.csv> <histo|leaks> <parametre>"

# Démarrer le chronomètre
START_MS=$(date +%s%3N)

# 1. Vérification du nombre d'arguments (Strictement 3)
if [ "$#" -ne 3 ]; then
    echo "ERREUR ($ERR_ARGS): Nombre d'arguments incorrect ($# reçu, 3 attendus)." >&2
    echo "$USAGE" >&2
    exit $ERR_ARGS
fi

CSV="$1"
ACTION="$2"
PARAM="$3"

# 2. Vérification du fichier CSV
if [ ! -f "$CSV" ]; then
    echo "ERREUR ($ERR_CSV): Fichier '$CSV' introuvable." >&2
    exit $ERR_CSV
fi

# 3. Compilation automatique
if [ ! -x build/eau_analyse ]; then
    echo "Compilation en cours..."
    if ! make; then
        echo "ERREUR ($ERR_COMPIL): La compilation a échoué." >&2
        exit $ERR_COMPIL
    fi
fi

EXECUTABLE="./build/eau_analyse"

case "$ACTION" in
    histo)
        if [[ "$PARAM" != "max" && "$PARAM" != "src" && "$PARAM" != "real" ]]; then
            echo "ERREUR ($ERR_H_MODE): Mode histo inconnu '$PARAM'. (max|src|real)" >&2
            exit $ERR_H_MODE
        fi

        echo "Traitement Histogramme en cours..."
        $EXECUTABLE histo "$CSV" "$PARAM"
        
        DAT="vol_${PARAM}.dat"
        if [ -f "$DAT" ]; then
            # Colonne à tracer : Max=2, Src=3, Real=4
            COL=2; [ "$PARAM" = "src" ] && COL=3; [ "$PARAM" = "real" ] && COL=4

            # Génération des 10 plus grandes
            tail -n +2 "$DAT" | sort -t',' -k2,2n | tail -n 10 > tmp_top.dat
            gnuplot -e "set terminal pngcairo; set output 'vol_${PARAM}_largest.png'; set title '10 plus grandes usines'; set ylabel 'M.m3'; set style fill solid; set xtic rotate by -45; set datafile separator ','; plot 'tmp_top.dat' using $COL:xtic(1) notitle with boxes"
            
            # Génération des 50 plus petites
            tail -n +2 "$DAT" | sort -t',' -k2,2n | head -n 50 > tmp_bot.dat
            gnuplot -e "set terminal pngcairo; set output 'vol_${PARAM}_smallest.png'; set title '50 plus petites usines'; set ylabel 'M.m3'; set style fill solid; set xtic rotate by -45; set datafile separator ','; plot 'tmp_bot.dat' using $COL:xtic(1) notitle with boxes"
            
            rm -f tmp_top.dat tmp_bot.dat
            echo "Graphiques générés : vol_${PARAM}_largest.png et vol_${PARAM}_smallest.png"
        fi
        ;;

    leaks)
        echo "Calcul des fuites pour l'usine : $PARAM"
        # On récupère juste la valeur numérique pour l'affichage
        RESULT=$($EXECUTABLE leaks "$CSV" "$PARAM")
        echo "Volume total des fuites (aval) : $RESULT M.m3"
        ;;

    *)
        echo "ERREUR ($ERR_ACTION): Action '$ACTION' inconnue." >&2
        exit $ERR_ACTION
        ;;
esac

# Fin du chronomètre
END_MS=$(date +%s%3N)
echo "Durée totale : $((END_MS - START_MS)) ms"
