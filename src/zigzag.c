#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zigzag.h"
#include <stdint.h>  
 /*
  La fonction inverse_zigzag reconstruit une matrice 8×8 à partir d'un vecteur de 64 éléments,
  en suivant l'ordre inverse du parcours en zigzag utilisé dans l'encodage JPEG.
  Elle se base sur la propriété suivante : toutes les positions (ligne, colonne) qui se trouvent
  sur une même diagonale ont la même somme ligne + colonne. Cette somme est représentée par la variable slc.
  Le parcours zigzag alterne entre deux types de diagonales :
  - Si la somme slc est paire : la diagonale est parcourue vers le haut (de bas-gauche à haut-droit)
  - Si slc est impaire : la diagonale est parcourue vers le bas (de haut-droit à bas-gauche)
  Pour chaque diagonale, on remplit les éléments valides (ligne et colonne < 8) de la matrice en
  prenant les valeurs du vecteur dans l'ordre, en avançant un indice `i` à chaque affectation.
 */

void inverse_zigzag(int* vecteur, int16_t **bloc) {
    // init
    int i = 0;
    int col=0;
    int ligne=0;
    int slc=0;
    // i est l'indice de vecteur qu'on veut inserer dans la matrice bloc.
    // slc représente somme de ligne et colonne qui va de 0 à 14 car on le premier pixel est (0,0) et le dernier est (7,7).
    for (slc = 0; slc <=14; slc++) { 
        if (slc % 2 == 0) { // cas où la somme de ligne et colonne est impaire, on est dans le cas de la diagonale montante.
            for (col = 0; col<= slc; col++) {
                ligne = slc - col;
                if (ligne < 8 && col < 8) {
                    bloc[ligne][col] = vecteur[i++];
                }
            }
        } else { // cas où la somme de ligne et colonne est impaire, on est dans le cas de la diagonale descentante.
            
            for (ligne = 0; ligne <= slc; ligne++) {
                col= slc - ligne;
                if (ligne < 8 && col < 8) {
                    bloc[ligne][col] = vecteur[i++];
                }
            }
        }
    }  
}



// int main() {
//     // 1. Créer un vecteur de 64 entiers avec des valeurs distinctes
//     int vecteur[64];
//     for (int i = 0; i < 64; i++) {
//         vecteur[i] = i;
//     }

//     // 2. Allouer dynamiquement une matrice 8x8
//     int **bloc = (int **)malloc(8 * sizeof(int *));
//     for (int i = 0; i < 8; i++) {
//         bloc[i] = (int *)malloc(8 * sizeof(int));
//     }

//     // 3. Appliquer la fonction inverse_zigzag
//     inverse_zigzag(vecteur, bloc);

//     // 4. Afficher la matrice résultat
//     printf("Résultat de inverse_zigzag :\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%3d ", bloc[i][j]);
//         }
//         printf("\n");
//     }

//     // 5. Libérer la mémoire
//     for (int i = 0; i < 8; i++) {
//         free(bloc[i]);
//     }
//     free(bloc);

//     return 0;
// }
// int main() {
//     // La chaîne hexadécimale fournie
//     const char *hex_values = "7c 0 fffa feb3 0 fee4 0 fedb 0 ff36 80 0 ff8b 0 0 0 6a 0 ffa0 0 ffa6 ff76 0 11c 0 45 0 ffec 0 ff7d 0 19 0 13 0 dd 0 1a 0 ff 0 9a 0 0 4 0 7d 0 ffa8 0 ff59 0 14 0 0 fe1f 0 ffb9 0 f4 0 0 ff3c 0";

//     // Convertir la chaîne hexadécimale en tableau d'entiers
//     int vecteur[64];  // Le vecteur doit contenir exactement 64 éléments
//     int i = 0;
//     unsigned int value;
//     const char *ptr = hex_values;
    
//     while (sscanf(ptr, "%x", &value) == 1) {
//         vecteur[i++] = value;
//         ptr = strchr(ptr, ' ');
//         if (ptr) ptr++;  // Passer à l'élément suivant
//     }

//     // Créer un bloc 8x8
//     int *bloc[8];
//     for (int i = 0; i < 8; i++) {
//         bloc[i] = (int *)malloc(8 * sizeof(int));
//     }

//     // Appeler la fonction inverse_zigzag
//     inverse_zigzag(vecteur, bloc);

//     // Afficher la matrice résultante
//     printf("Matrice 8x8 reconstruite:\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%04x ", bloc[i][j]);
//         }
//         printf("\n");
//     }

//     // Libérer la mémoire allouée pour le bloc
//     for (int i = 0; i < 8; i++) {
//         free(bloc[i]);
//     }

//     return 0;
// }