#include "zigzag.h"
#include <stdint.h>  
 /*
  La fonction inverse_zigzag reconstruit une matrice 8×8 bloc à partir d'un vecteur de 64 éléments.
  Elle se base sur la propriété suivante : toutes les positions (ligne, colonne) qui se trouvent
  sur une même diagonale ont la même somme ligne + colonne. Cette somme est représentée par la variable slc.
  Le parcours zigzag alterne entre deux types de diagonales :
  * Si la somme slc est paire : la diagonale est parcourue vers le haut (de bas-gauche à haut-droit)
  * Si slc est impaire : la diagonale est parcourue vers le bas (de haut-droit à bas-gauche)
  Pour chaque diagonale, on remplit les éléments valides (ligne et colonne < 8) de la matrice en
  prenant les valeurs du vecteur dans l'ordre, en avançant un indice i à chaque affectation.
 */

void inverse_zigzag(int* vecteur, int16_t **bloc) {
    // initialisation des variables 
    int i = 0; // i est l'indice de vecteur qu'on veut inserer dans la matrice bloc.
    int col=0;
    int ligne=0;
    int slc=0;
    // slc représente somme de ligne et colonne qui est entre 0 et 14, car le premier pixel est (0,0) et le dernier est (7,7).
    for (slc = 0; slc <=14; slc++) { 
        if (slc % 2 == 0) { // cas où la somme de ligne et colonne est paire, on est dans le cas de la diagonale montante.
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

//Test 
// int main() {
//     int vecteur[64];
//     for (int i = 0; i < 64; i++) {
//         vecteur[i] = i;
//     }     
//     int **bloc = (int **)malloc(8 * sizeof(int *));
//     for (int i = 0; i < 8; i++) {
//         bloc[i] = (int *)malloc(8 * sizeof(int));
//     }     
//     inverse_zigzag(vecteur, bloc);    
//     printf("Résultat:\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%3d ", bloc[i][j]);
//         }
//         printf("\n");
//     }
//     for (int i = 0; i < 8; i++) {
//         free(bloc[i]);
//     }
//     free(bloc);

//     return 0;
// }
// int main() {
//     const char *hex_values = "7c 0 fffa feb3 0 fee4 0 fedb 0 ff36 80 0 ff8b 0 0 0 6a 0 ffa0 0 ffa6 ff76 0 11c 0 45 0 ffec 0 ff7d 0 19 0 13 0 dd 0 1a 0 ff 0 9a 0 0 4 0 7d 0 ffa8 0 ff59 0 14 0 0 fe1f 0 ffb9 0 f4 0 0 ff3c 0";
//     int vecteur[64];  // Le vecteur doit contenir exactement 64 éléments
//     int i = 0;
//     unsigned int value;
//     const char *ptr = hex_values;  
//     while (sscanf(ptr, "%x", &value) == 1) {
//         vecteur[i++] = value;
//         ptr = strchr(ptr, ' ');
//         if (ptr) ptr++;  // Passer à l'élément suivant
//     }
//     int *bloc[8];
//     for (int i = 0; i < 8; i++) {
//         bloc[i] = (int *)malloc(8 * sizeof(int));
//     }
//     inverse_zigzag(vecteur, bloc);
//     printf("Matrice 8x8 reconstruite:\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%04x ", bloc[i][j]);
//         }
//         printf("\n");
//     }
//     for (int i = 0; i < 8; i++) {
//         free(bloc[i]);
//     }
//     return 0;
// }