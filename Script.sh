#!/usr/bin/env bash
set -euo pipefail

# Définition des codes d'erreur
ERR_ARGS=1
ERR_CSV=2
ERR_COMPIL=3
ERR_H_ARGS=4
ERR_H_MODE=5
ERR_L_ARGS=6
ERR_C_FAIL=7
ERR_ACTION=8

USAGE="Usage: $0 <fichier.csv> histo <max|src|real>  OR  $0 <fichier.csv> leaks \"FactoryID\""

# Démarrer le chronomètre
START_MS=$(date +%s%3N)

if [ $# -lt 3 ]; then
    echo "ERREUR ($ERR_ARGS): arguments insuffisants." >&2
    echo "$USAGE" >&2
    exit $ERR_ARGS
fi

CSV="$1"
ACTION="$2"
PARAM="$3"

# 1. Vérification stricte du nombre d'arguments
if [ "$ACTION" = "histo" ] && [ "$#" -ne 3 ]; then
    echo "ERREUR ($ERR_H_ARGS): histo requiert 3 arguments au total (fichier, histo, mode)." >&2
    echo "$USAGE" >&2
    exit $ERR_H_ARGS
fi

if [ "$ACTION" = "leaks" ] && [ "$#" -ne 3 ]; then
    echo "ERREUR ($ERR_L_ARGS): leaks requiert 3 arguments au total (fichier, leaks, ID)." >&2
    echo "$USAGE" >&2
    exit $ERR_L_ARGS
fi

# 2. Vérification du fichier CSV
if [ ! -f "$CSV" ]; then
    echo "ERREUR ($ERR_CSV): fichier $CSV introuvable." >&2
    exit $ERR_CSV
fi

# 3. Compile si nécessaire
if [ ! -x build/eau_analyse ]; then
    echo "Compilation via make..."
    if ! make; then
        echo "ERREUR ($ERR_COMPIL): compilation échouée." >&2
        exit $ERR_COMPIL
    fi
fi

EXECUTABLE="build/eau_analyse"

case "$ACTION" in
    histo)
        MODE="$PARAM"
        if [[ "$MODE" != "max" && "$MODE" != "src" && "$MODE" != "real" ]]; then
            echo "ERREUR ($ERR_H_MODE): mode inconnu $MODE. Doit être max, src ou real." >&2
            exit $ERR_H_MODE
        fi

        # Appel du programme C pour générer les données (triées par ID alphabétique inverse)
        echo "Génération des données : vol_${MODE}.dat"
        "$EXECUTABLE" histo "$CSV" "$MODE"
        C_RETURN_CODE=$?
        if [ "$C_RETURN_CODE" -ne 0 ]; then
            echo "ERREUR ($ERR_C_FAIL): Le programme C (histo) a retourné un code d'erreur ($C_RETURN_CODE)." >&2
            exit $ERR_C_FAIL
        fi
        
        DAT="vol_${MODE}.dat"
        if [ -f "$DAT" ] && command -v gnuplot >/dev/null 2>&1; then
            
            # Déterminer la colonne à tracer dans GnuPlot (Max=2, Src=3, Real=4)
            COL_TO_PLOT=
            TITLE_Y="volume (k.m3.year-1)"
            if [ "$MODE" = "max" ]; then COL_TO_PLOT=2
            elif [ "$MODE" = "src" ]; then COL_TO_PLOT=3
            elif [ "$MODE" = "real" ]; then COL_TO_PLOT=4
            fi

            # --- 4.1 Préparation et tracé des 10 plus grandes (référence : Max Volume) ---
            # Saute l'en-tête (tail -n +2) | trie numériquement sur la colonne 2 (max volume) | garde les 10 derniers (plus grands)
            tail -n +2 "$DAT" | sort -t',' -k2,2n | tail -n 10 > temp_10_largest.dat

            gnuplot -persist << EOL
set terminal pngcairo size 1200,600 enhanced font 'Arial,10'
set output "vol_${MODE}_largest.png"
set title "10 plus grandes usines (réf: Capacité Max)"
set ylabel "$TITLE_Y"
set style data histogram
set style fill solid 1.00 border -1
set boxwidth 0.9
set xtic rotate by -45 scale 0
set datafile separator ","
plot 'temp_10_largest.dat' using $COL_TO_PLOT:xtic(1) title "${MODE} volume"
EOL
            echo "PNG (10 plus grandes) : vol_${MODE}_largest.png"

            # --- 4.2 Préparation et tracé des 50 plus petites (référence : Max Volume) ---
            # Saute l'en-tête | trie numériquement sur la colonne 2 (max volume) | garde les 50 premiers (plus petits)
            tail -n +2 "$DAT" | sort -t',' -k2,2n | head -n 50 > temp_50_smallest.dat

            gnuplot -persist << EOL
set terminal pngcairo size 1200,600 enhanced font 'Arial,10'
set output "vol_${MODE}_smallest.png"
set title "50 plus petites usines (réf: Capacité Max)"
set ylabel "$TITLE_Y"
set style data histogram
set style fill solid 1.00 border -1
set boxwidth 0.9
set xtic rotate by -45 scale 0
set datafile separator ","
plot 'temp_50_smallest.dat' using $COL_TO_PLOT:xtic(1) title "${MODE} volume"
EOL
            echo "PNG (50 plus petites) : vol_${MODE}_smallest.png"

            rm temp_10_largest.dat temp_50_smallest.dat
        else
            echo "ATTENTION: gnuplot est absent ou fichier de données '$DAT' manquant. La visualisation n'a pas été effectuée."
        fi
        ;;
    leaks)
        FAC="$PARAM"
        
        # Le C écrit l'historique et affiche la fuite sur stdout
        OUTPUT=$("$EXECUTABLE" leaks "$CSV" "$FAC")
        C_RETURN_CODE=$?

        if [ "$C_RETURN_CODE" -ne 0 ]; then
            echo "ERREUR ($ERR_C_FAIL): Le programme C (leaks) a retourné un code d'erreur ($C_RETURN_CODE)." >&2
            exit $ERR_C_FAIL
        fi
        
        # Affichage du résultat, comme demandé par le sujet
        echo "Résultat de fuite pour '$FAC' (M.m3) : $OUTPUT"
        ;;
    *)
        echo "ERREUR ($ERR_ACTION): action inconnue $ACTION" >&2
        echo "$USAGE" >&2
        exit $ERR_ACTION
        ;;
esac

# Afficher la durée à la fin
END_MS=$(date +%s%3N)
DURATION_MS=$((END_MS - START_MS))
echo "Durée totale : ${DURATION_MS} ms"
