#include <stdio.h>
#include "transformation_data.h"  
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

void inverse_zigzag(int* vecteur, int **bloc) {
    // init
    int i = 0;
    int col=0;
    int ligne=0;
    int slc=0;
    // i est l'indice de vecteur qu'on veut inserer dans la matrice bloc.
    // slc représente somme de ligne et colonne qui va de 0 à 14 car on le premier pixel est (0,0) et le dernier est (7,7).
    for (slc = 0; slc < 15; slc++) { 
        if (slc % 2 == 0) { // cas où la somme de ligne et colonne est paire, on est dans le cas de la diagonale montante.
            for (ligne = 0; ligne <= slc; ligne++) {
                col = slc - ligne;
                if (ligne < 8 && col < 8) {
                    bloc[ligne][col] = vecteur[i++];
                }
            }
        } else { // cas où la somme de ligne et colonne est impaire, on est dans le cas de la diagonale descentante.
            
            for (col = 0; col <= slc; col++) {
                ligne = slc - col;
                if (ligne < 8 && col < 8) {
                    bloc[ligne][col] = vecteur[i++];
                }
            }
        }
    }  
}