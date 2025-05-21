#!/bin/bash

# Définition des codes couleur
BLUE='\033[0;34m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BOLD='\033[1m'
RESET='\033[0m'

# Fonctions d'affichage
print_header() {
    echo -e "${BLUE}${BOLD}=======================================${RESET}"
    echo -e "${BLUE}${BOLD}        CONVERTISSEUR JPEG            ${RESET}"
    echo -e "${BLUE}${BOLD}=======================================${RESET}"
    echo
}

print_success() {
    echo -e "${GREEN}✓ $1${RESET}"
}

print_error() {
    echo -e "${RED}✗ $1${RESET}"
}

print_section() {
    echo -e "${YELLOW}${BOLD}>>> $1${RESET}"
}

print_separator() {
    echo -e "${BLUE}------------------------------${RESET}"
}

# Début du script
clear
print_header
# Nettoyage
print_section "Nettoyage"
rm -f ./images/*.ppm ./images/*.pgm
rm -f ./images/*.blabla
print_success "Nettoyage terminé"
# Compilation
print_section "Compilation du programme"
make clean > /dev/null
make > /dev/null
print_success "Compilation terminée"
print_separator

# Répertoire de sortie
OUTPUT_DIR="./images"

# Recherche des fichiers JPEG
print_section "Recherche de fichiers JPEG dans ./images/"
jpeg_files=($(find ./images/ -type f -iname "*.jp*g" | sort))
total_files=${#jpeg_files[@]}

if [ $total_files -eq 0 ]; then
    print_error "Aucun fichier JPEG trouvé"
    exit 1
else
    print_success "$total_files fichiers JPEG trouvés"
    for file in "${jpeg_files[@]}"; do
        echo "  - $(basename "$file")"
    done
fi
print_separator

read -p "Appuyez sur Entrée pour commencer..."

# Traitement des fichiers
print_section "Traitement des fichiers"
count=0
current=0

for file in "${jpeg_files[@]}"; do
    # Mise à jour du compteur
    current=$((current+1))
    # Nom de base
    filename=$(basename "$file")
    base="${filename%.*}"
    
    # Afficher la progression
    progress=$((current * 100 / total_files))
    echo -e "${YELLOW}[$progress%] Traitement de: $filename${RESET}"
    
    # Exécution du convertisseur
    ./jpeg2ppm "$file"
    jpeg2ppm_exit_code=$?
    
    if [ $jpeg2ppm_exit_code -ne 0 ]; then
        print_error "Erreur sur $filename (code: $jpeg2ppm_exit_code)"
        print_separator
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
        print_error "Aucun fichier de sortie trouvé pour $filename"
        print_separator
        continue
    fi

    print_success "Conversion réussie de $filename en $output_file"
    count=$((count+1))
    feh --fullscreen "$OUTPUT_DIR/$output_file" # Ouvre l'image convertie
    # eog "$OUTPUT_DIR/$output_file" # Ouvre l'image convertie
    print_separator
    # read -p "Appuyez sur Entrée pour continuer..."
done

# Résumé
print_section "Résumé"
if [ $count -eq $total_files ]; then
    print_success "Conversion terminée : $count fichiers sur $total_files"
else
    print_error "Conversion terminée : $count fichiers sur $total_files"
fi

read -p "Appuyez sur Entrée pour continuer..."

# Nettoyage
print_section "Nettoyage"
rm -f ./images/*.ppm ./images/*.pgm
rm -f ./images/*.blabla
make clean > /dev/null
print_success "Nettoyage terminé"

read -p "Appuyez sur Entrée pour quitter..."
clear
