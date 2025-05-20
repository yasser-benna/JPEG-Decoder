# Projet décodeur JPEG

Dans ce projet on a pu implémenter un décodeur JPEG en langage C. Il permet de décompresser des images compressées au format JPEG et de les restituer au format pgm (niveaux de gris) ou ppm (couleurs), en respectant le standard JPEG.

------------------------------------------------------------------------

## Répartition des tâches et contributions

### Contributions individuelles

**EL CHAMAA MOHAMAD**

\- Implémentation de la lecture des en-têtes JPEG

\- Développement du système de gestion de BitStream

\- Conception et implémentation de l'écriture en format PPM

\- Organisation et distribution des composantes RGB dans la matrice finale

\- Optimisation de la gestion mémoire et refactorisation du code

\- Établissement de l'architecture logicielle et gestion des dépendances

**BENNA YASSER**

-Implémentation de l'algorithme de quantification inverse

\- Développement des versions naïve et optimisée de la transformation DCT inverse

\- Conception et réalisation du système d'up-sampling

\- Contribution du décodage Huffman - Développement de la conversion YCbCr vers RGB

**TROMBATI MOHAMED**

Implémentation de l'algorithme zig-zag inverse

\- Elaboration du décodage Huffman

\- Conception du système d'extraction de blocs

\- Développement de la conversion YCbCr vers RGB

### Méthodologie de travail

Notre approche a été fondée sur une collaboration étroite et régulière. Les sessions de travail se sont principalement déroulées dans les salles de TP, permettant une communication directe et efficace entre les membres de l'équipe.

La répartition des tâches a été effectuée en tenant compte des compétences de chacun tout en veillant à maintenir un équilibre dans la charge de travail. Cette organisation nous a permis d'avancer méthodiquement et de minimiser les retards potentiels.

------------------------------------------------------------------------

### Details pour chaque mission

#### Gestion du BitStream (bitsmanip.c)

Le module de manipulation de bits (`bitsmanip.c`) constitue une composante fondamentale du décodeur JPEG, permettant l'accès précis aux données compressées au niveau binaire. 
L'implémentation repose sur une structure `BitStream` qui encapsule efficacement l'état de lecture du flux de bits : un pointeur de fichier (`fp`), un tampon de 8 bits (`buffer`), un compteur de bits disponibles (`bits_available`), un compteur d'octets lus (`bytes_read`) et un indicateur de fin de fichier (`is_eof`). 
L'architecture du module suit un modèle d'encapsulation robuste avec des fonctions d'initialisation (`bitstream_init`) et de nettoyage (`bitstream_close`) qui gèrent respectivement l'allocation/désallocation mémoire et l'ouverture/fermeture du fichier source. 
Le cœur fonctionnel réside dans le mécanisme de lecture binaire, articulé autour de quatre opérations principales : (1) `bitstream_fill_buffer` qui approvisionne automatiquement le tampon lorsqu'il est vide, (2) `bitstream_read_bit` qui extrait un bit unique du tampon avec une gestion appropriée du décalage, (3) `bitstream_read_bits` qui compose plusieurs bits en une valeur unique via une accumulation séquentielle, et (4) `bitstream_peek_bits` qui permet une lecture non-destructive en sauvegardant et restaurant l'état du flux. 
Cette dernière fonction est particulièrement cruciale pour l'analyse prospective des codes Huffman. 
L'implémentation intègre également une couche d'abstraction via des accesseurs (`get_fp`, `get_buffer`, etc.) qui renforcent l'encapsulation des données et facilitent la maintenance. 
L'ensemble du module démontre une conception soignée qui équilibre efficacité computationnelle et clarté structurelle, fournissant ainsi une interface fiable pour le traitement bit par bit des données JPEG compressées.

#### Extraction des blocs (extraction_bloc.c)

Le module d'extraction de blocs constitue une étape centrale dans le processus de décodage JPEG, responsable de la reconstruction des unités fondamentales de l'image compressée. 
L'implémentation repose sur une fonction principale `extraire_bloc` qui orchestre méthodiquement le décodage des MCUs (Minimum Coded Units) à partir du flux de données compressées. 
Le processus commence par une analyse précise de la structure spatiale de l'image en calculant les dimensions des MCUs (basées sur les facteurs d'échantillonnage horizontal H_Y et vertical V_Y de la composante de luminance) et en déterminant le nombre total de MCUs nécessaires pour couvrir l'intégralité de l'image. 
L'architecture mémoire adoptée est hiérarchique et adaptative : pour chaque composante (Y, Cb, Cr), une structure tridimensionnelle est allouée dynamiquement pour stocker les coefficients DCT de chaque bloc 8×8 dans chaque MCU. 
Le décodage procède séquentiellement par MCU, en traitant successivement tous les blocs de chaque composante colorimétriques.
Pour chaque bloc, la fonction invoque `decoder_bloc` qui utilise les tables de Huffman spécifiques à chaque composante et type de coefficient (DC ou AC) pour reconstituer les valeurs quantifiées. 
Un aspect crucial de l'algorithme est la gestion des coefficients DC, qui sont encodés différentiellement : le module maintient donc des variables d'état (`dc_prec_Y`, `dc_prec_Cb`, `dc_prec_Cr`) pour assurer la cohérence du décodage entre blocs successifs. 
L'implémentation démontre une attention particulière à l'optimisation de la mémoire, avec une allocation précise adaptée aux facteurs d'échantillonnage spécifiques de chaque composante, permettant ainsi de gérer efficacement aussi bien les images en sous-échantillonnage 4:2:0 que d'autres formats comme 4:2:2 ou 4:4:4. 
Cette conception modulaire facilite l'intégration avec les étapes ultérieures du traitement, notamment le zig-zag inverse, la quantification inverse et l'IDCT.

#### Décodage Huffman (huffman.c)

Le module de décodage Huffman représente une composante critique du décodeur JPEG, responsable de la transformation des flux de bits compressés en symboles significatifs. 
L'implémentation repose sur l'exploitation de la nature des codes à longueur variable où les symboles fréquents sont représentés par des codes courts, et les symboles rares par des codes plus longs. 
La structure centrale du module est organisée autour de deux fonctions principales: la construction des tables de décodage et l'extraction des symboles du flux binaire. 
La fonction `construire_table_huffman` analyse systématiquement les spécifications de longueurs de codes (huffsize) et les valeurs de codes (huffcode) fournies dans l'en-tête JPEG pour générer des tables de correspondance efficaces. 
Ces tables sont structurées sous forme d'arbres binaires où chaque chemin représente une séquence de bits et chaque feuille un symbole. 
La seconde fonction fondamentale, `decoder_bloc`, exploite ces tables pour extraire séquentiellement les coefficients DCT de chaque bloc 8×8. Cette fonction gère deux types distincts de coefficients: les coefficients DC, encodés différentiellement (la différence avec le coefficient DC du bloc précédent), et les coefficients AC, potentiellement suivis par des séquences de zéros (RLE - Run Length Encoding). 
Le processus de décodage procède par une analyse bit par bit du flux entrant: à chaque étape, le décodeur tente de faire correspondre la séquence de bits en cours avec un code Huffman valide. 
Une fois la correspondance établie, le symbole associé est interprété, soit directement (pour les coefficients AC de faible amplitude), soit comme une indication de la longueur en bits de la valeur suivante (particulièrement important pour les coefficients DC). 
L'implémentation intègre également un mécanisme sophistiqué de gestion des cas particuliers comme le symbole de fin de bloc (EOB) qui signale que tous les coefficients AC restants sont nuls, ainsi que les codes spéciaux indiquant des séquences de zéros. 
Cette conception équilibre efficacement la vitesse de décodage et la précision, permettant une reconstruction fidèle des données image originales à partir du flux compressé.




