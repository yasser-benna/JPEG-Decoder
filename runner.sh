#!/bin/bash
make clean > /dev/null
make > /dev/null
# Répertoire de sortie
OUTPUT_DIR="./images"

# Compteur
count=0

# Trouver tous les fichiers JPEG (insensible à la casse)
echo "Recherche de fichiers JPEG dans ./images/"
find ./images/ -type f -iname "*.jp*g" | sort

echo "------------------------------"

read -p "Appuyez sur Entrée pour commencer..."

# Process all files with case-insensitive matching
find ./images/ -type f -iname "*.jp*g" | sort | while read file
do
    # Nom de base
    filename=$(basename "$file")
    base="${filename%.*}"

    echo "Traitement de: $file"

    # Exécution du convertisseur
    ./jpeg2ppm "$file"
    jpeg2ppm_exit_code=$?

    if [ $jpeg2ppm_exit_code -ne 0 ]; then
        echo "Erreur sur $filename (code: $jpeg2ppm_exit_code)"
        echo "------------------------------"
        continue
    fi

    # Noms de sortie possibles
    ppm_output="${base}.ppm"
    pgm_output="${base}.pgm"

    # Vérifie et déplace si besoin
    if [ -f "$OUTPUT_DIR/$ppm_output" ]; then
        output_file="$ppm_output"
    elif [ -f "$OUTPUT_DIR/$pgm_output" ]; then
        output_file="$pgm_output"
    else
        echo "Aucun fichier de sortie trouvé pour $filename"
        echo "------------------------------"
        continue
    fi

    echo "Conversion réussie de $filename en $output_file"
    count=$((count+1))

    # eog "$OUTPUT_DIR/$output_file"
    echo "------------------------------"
done

# Résumé
echo "Conversion terminée : $count fichiers sur $total_files."
read -p "Appuyez sur Entrée pour continuer..."
# Nettoyage
rm -f "$TMP_FILE_LIST"
rm -f ./images/*.ppm ./images/*.pgm
rm ./images/*.blabla
make clean > /dev/null
read -p "Appuyez sur Entrée pour quitter..."
clear
