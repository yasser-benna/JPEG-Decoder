#include <stdlib.h> 
#include <stdint.h>   
#include <string.h>  
#include "huffman.h"
#include "bitsmanip.h"

/*La fonction read_bit permet de lire bit par bit à partir d'une position. Elle traite aussi le cas 
du Byte Stuffing mentionné dans le sujet .*/

uint8_t read_bit(const uint8_t *data, size_t *bit_pos) {

    static size_t precedent_octet = SIZE_MAX; // Mémorise l'octet précédent pour détecter le byte stuffing
    size_t indice_byte = *bit_pos / 8; // l'indice de l'octet 
    size_t bit_offset = *bit_pos % 8;  // l'emplacement du bit dans cet octet 

    // Cas du Byte Stuffing, quand on détécte 0xFF 0x00, on jette cet octet nul . 
    if (indice_byte != precedent_octet && indice_byte > 0 &&
        data[indice_byte - 1] == 0xFF && data[indice_byte] == 0x00) {
        indice_byte++;
        *bit_pos += 8; 
    }

    // Vérifie si l'indice de l'octet est valide
    if (indice_byte >= SIZE_MAX) {
        precedent_octet = indice_byte;
    }

    uint8_t byte = data[indice_byte]; // On récupère l'octet de la data 
    uint8_t bit = (byte >> (7 - bit_offset)) & 1; // On récupère le bit voulu 

    (*bit_pos)++; // incrémentation 
    precedent_octet = indice_byte; // nouvelle valeur de l'octet précedent 
    return bit;
}

/*La fonction read_bits lit n bits successifs à partir d'une certaine position, ces n bits forme une 
une valeur uint32_t qu'on retourne à la fin .*/

uint32_t read_bits(const uint8_t *data, size_t *bit_pos, int n) {
    // initialisation de la valeur qu'on veut retourner après lecture des n bits 
    uint32_t valeur = 0;
    // lecture de n bits 
    for (int i = 0; i < n; i++) {
        valeur = (valeur << 1) | read_bit(data, bit_pos); // mise à jour de valeur, après l'ajout d'un nouveau bit à droite
    }
    return valeur;
}

/* La fonction generer_codes_huffman permet de générer les code de huffman à partir de la liste des longeurs des 
des codes et aussi l'ensemble des symboles. Ces données sont fournies dans la partie DHT du format JPEG .*/

char **generer_codes_huffman(uint8_t taille[16], int n_symboles) {
    // initialisation des variables 
    int i =0;
    int j=0;
    int k= 0;   
    int code = 0;
    int longueur_code=1;
    int nbr_code;
    char **table_huffman = malloc(n_symboles * sizeof(char *)); // Allocation pour la table des codes Huffman

    // ici on parcourt de 1 à 16, car un code ne peut pas avoir une taille supérieur à 16 bits 
    for (longueur_code = 1; longueur_code <= 16; longueur_code++) {
        nbr_code = taille[longueur_code - 1];
        // On construit nbr_code codes de la longueur longueur_code
        for (j = 0; j < nbr_code; j++) {
            if (i >= n_symboles) {
                break;  
            }
            table_huffman[i] = malloc((longueur_code + 1) * sizeof(char)); // +1 pour le caractère nul
            for (k = 0; k < longueur_code; k++) {
                table_huffman[i][k] = ((code >> (longueur_code - 1 - k)) & 1) + '0'; // méthode qui permet de génére code de Huffman connaissant seulement la longueur du code 
            }
            table_huffman[i][longueur_code] = '\0'; // Termine la chaîne de caractères
            i++; // incrémentation pour passer au prchain symbole
            code++; // incrémentation pour passer au prochain code 
        }
        // on ajoute 0 à droite pour la longeur de code suivante 
        code <<= 1;
    }

    return table_huffman;
    
}

// Fonction pour libérer la mémoire allouée pour la table de Huffman
void free_huffman_table(char **table_huffman, int taille) {
    for (int i = 0; i < taille; i++) {
        free(table_huffman[i]); 
    } 
    free(table_huffman);
}


/*La fonction decoder_huffman permet de décoder des données lues à partir de la tables de Huffman générée. */

int decoder_huffman(const uint8_t *data, size_t *bit_pos, char **table_huffman, int n_symboles) {
    // initialisation 
    int compteur_de_bit = 0; // compteur des bits lus qui ne peut dépasser 16 bits
    char code_lu[17] = {0};  // code lu qui ne peut dépasser 17, car on a maximum 16 bits en plus du \0 qu'on ajoute à la fin
    
    while (compteur_de_bit < 16) {

        uint8_t bit_courant = read_bit(data, bit_pos); // lecture de bit 
        code_lu[compteur_de_bit] = bit_courant + '0'; // conversion le bit en caractère 
        code_lu[compteur_de_bit + 1] = '\0'; // Termine la chaîne pour la comparaison
        compteur_de_bit++;
        for (int i = 0; i < n_symboles; i++) {
            if (strcmp(code_lu, table_huffman[i]) == 0) {
                return i;  // si le code lu se trouve dans la table de Huffman, on retourne son indice dans cette table
            }
        }
    }
    return -1; // Code non trouvé dans la table
}


/*La fonction magnitude convertit une valeur codée selon le format de magnitude fournie dans le sujet .*/

int magnitude(int taille, uint32_t valeur) {
    // initialisation
    uint32_t nbr_critique;
    // si la taille de valeur est 0, on retourne 0
    if (taille == 0){
        return 0;  
    }
    nbr_critique = 1 << (taille - 1); // cet entier correspond au plus petit entier positif en terme de magnitude, c'est celui qui contient 1 à gauche et que des 0 à droite.
    if (valeur < nbr_critique)
        return valeur - ((1 << taille) - 1); // Si valeur inférieure strictement à nbr_critique, cela signifie que le bit de poids fort est 0, donc elle est négative dans le codage de magnitude.
    else 
        return valeur; // si valeur supérieure ou égale à nbr_critique alors elle est positive 
}

/*La fonction decoder_bloc permet de décoder un bloc à partir des données compréssées */

void decoder_bloc(
    const uint8_t *data, size_t *bit_pos,
    char **table_huffman_dc, uint8_t symboles_dc[256], int nbr_sym_dc,
    char **table_huffman_ac, uint8_t symboles_ac[256], int nbr_sym_ac,
    int *dc_precedent, int bloc[64]) {

    // initilisation de tous les cofficient du vecteur bloc à zéro, car on a un coefficient DC et 63 coefficient AC
    for (int i = 0; i <= 63; i++) {
        bloc[i] = 0; 
    }
    // Décodage du coefficient DC
    int indice_dc = decoder_huffman(data, bit_pos, table_huffman_dc, nbr_sym_dc);
    uint8_t taille_dc = symboles_dc[indice_dc]; // la taille du DC à lire
    uint32_t bits_dc = read_bits(data, bit_pos, taille_dc); // lecture des taille_dc bits en utilisant la fonction read_bits 
    int valeur_dc = magnitude(taille_dc, bits_dc); // conversion en terme de magnitude 
    bloc[0] = *dc_precedent + valeur_dc; // ajout de valeur_dc à dc_precedent 
    *dc_precedent = bloc[0];

    // Décodage des 63 coefficients AC
    int j = 1;
    while (j <= 63) {
        int indice_ac = decoder_huffman(data, bit_pos, table_huffman_ac, nbr_sym_ac);
        uint8_t symbole_ac = symboles_ac[indice_ac];
        uint8_t run = symbole_ac >> 4; // 4 bits de poids fort, qui indique le nombre de 0 
        uint8_t taille = symbole_ac & 0x0F; // 4 bits de poids faible qui indique la taille 
        // Cas End Of Bloc 
        if (symbole_ac == 0x00) {
            break;
        }
        if (symbole_ac == 0xF0) {
            j += 15; // Comme mentionné dans le sujet lorsqu'on a le symbole 0xF0, cela désigne le saut de 16 composantes nulles.
        } else {
            j += run; // sinon saut de run composantes nulle
        }
        if (j >= 64) {
            break;
        }
        uint32_t bits_ac = read_bits(data, bit_pos, taille); // lecture des taille_ac bits en utilisant la fonction read_bits 
        int valeur_ac = magnitude(taille, bits_ac);  // conversion en terme de magnitude 
        bloc[j] = valeur_ac; // ajout de valeur_ac à la j ième position
        j++; // incrémentation 
    }
}
