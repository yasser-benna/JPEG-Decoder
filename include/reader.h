#pragma once
#include <stdint.h>
#include "bitsmanip.h"
// Structure pour stocker les informations des composants SOF0
typedef struct COMPONENT{
    uint8_t id;          // Identifiant du composant
    uint8_t h_factor;    // Facteur d'échantillonnage horizontal
    uint8_t v_factor;    // Facteur d'échantillonnage vertical
    uint8_t qt_id;       // Identifiant de la table de quantification
} COMPONENT;
// Structure pour stocker les informations des tables de Huffman
typedef struct HUFF_TAB{
    uint8_t id;          // Identifiant de la table
    uint8_t ac_dc;       // Type de table (AC ou DC)
    uint8_t symbols[256]; // Symboles de la table
    uint8_t tailles[16];  // Tailles des codes
    int nb_symbols;      // Nombre de symboles
}HUFFMAN_TABLE;
// Structure pour stocker les informations des composants SOS
typedef struct {
    uint8_t id;           // Identifiant du composant
    uint8_t dc_table_id;  // Identifiant de la table DC
    uint8_t ac_table_id;  // Identifiant de la table AC
} SOS_COMPONENT;

// Structure pour stocker les informations de l'image
typedef struct IMAGE{
    int APPX;             // Marqueur APP
    char* COMMENTAIRE;    // Commentaire de l'image
    uint16_t* Quant_Table[4]; // Tables de quantification
    uint16_t Hauteur;     // Hauteur de l'image
    uint16_t Largeur;     // Largeur de l'image
    uint8_t nb_components; // Nombre de composants
    COMPONENT COMPONENTS[4]; // Composants SOF0
    HUFFMAN_TABLE HUFFMAN_tables[4]; // Tables de Huffman
    uint8_t nb_components_sos; // Nombre de composants SOS
    SOS_COMPONENT COMPONENTS_SOS[4]; // Composants SOS
    uint8_t Ss, Se;       // Paramètres de sélection spectrale
    uint8_t Ah, Al;       // Paramètres d'approximation successive
    uint8_t* compressed_data; // Données compressées
    size_t compressed_size; // Taille des données compressées
}IMAGE;



// Fonction pour initialiser la structure de l'image
IMAGE* init_image();
// Fonction pour libérer la mémoire d'une image
void free_image(IMAGE* img); 
// Fonction pour lire un fichier JPEG
IMAGE* read_jpeg(const char* file_name);

// Fonction pour lire le marqueur SOI (Start Of Image)
void read_soi(BitStream*bs);
// Fonction pour lire le marqueur EOI (End Of Image)
void read_eoi(BitStream*bs);
// Fonction pour lire le marqueur APPx
void read_appx(BitStream*bs,IMAGE*image);
// Fonction pour lire le marqueur COM (Commentaire)
void read_com(BitStream*bs,IMAGE*image);
// Fonction pour lire le marqueur DQT (Define Quantization Table)
void read_dqt(BitStream*bs,IMAGE*image);
// Fonction pour lire le marqueur SOFx (Start Of Frame)
void read_sofx(BitStream*bs,IMAGE*image);
// Fonction pour lire le marqueur DHT (Define Huffman Table)
void read_dht(BitStream*bs,IMAGE*image);
// Fonction pour lire le marqueur SOS (Start Of Scan)
void read_sos(BitStream*bs,IMAGE*image);
// Fonction pour afficher les informations de l'image
void print_image(const IMAGE* image);