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

#### Lecture des en-têtes JPEG (reader.c)

Le module de lecture des en-têtes JPEG constitue la pierre angulaire initiale du décodeur, responsable de l'interprétation correcte des métadonnées complexes qui préfigurent les données d'image compressées. 
L'implémentation s'articule autour d'un ensemble structuré de fonctions spécialisées pour l'analyse des différents segments de marqueurs JPEG. 
La fonction `read_header` orchestre le processus global, identifiant et traitant séquentiellement les marqueurs SOI (Start Of Image), APP0 (Application data), DQT (Define Quantization Table), SOF0 (Start Of Frame), DHT (Define Huffman Table), SOS (Start Of Scan) et autres segments essentiels. 
Pour chaque type de segment, des fonctions dédiées (`process_APP0`, `process_SOF0`, etc.) dissèquent minutieusement les données binaires selon les spécifications du standard JPEG. 
L'architecture démontre une approche robuste face à la diversité des encodages JPEG : le module gère avec précision l'extraction et l'interprétation de paramètres cruciaux comme les dimensions de l'image, le nombre de composantes colorimétriques, les facteurs d'échantillonnage horizontal et vertical pour chaque composante, les tables de quantification et les spécifications des tables de Huffman. 
Un soin particulier est accordé à la vérification de la validité des données extraites, avec des contrôles de cohérence appliqués aux identificateurs de composantes, aux indices de tables et aux autres paramètres critiques. 
Le module met en œuvre une gestion sophistiquée de la mémoire pour stocker efficacement les multiples tables et structures de données nécessaires aux étapes ultérieures du décodage. 
Les tables de quantification sont organisées dans des tableaux bidimensionnels, tandis que les spécifications des codes Huffman sont méticuleusement extraites pour faciliter la construction ultérieure des arbres de décodage. 
Ce composant illustre une mise en œuvre méthodique du standard JPEG, équilibrant fidélité aux spécifications et adaptabilité face aux variations d'implémentation que l'on peut rencontrer dans des fichiers JPEG de sources diverses. 
Son rôle est déterminant dans la configuration correcte de tous les paramètres qui conditionnent le succès des étapes subséquentes du processus de décodage.

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

#### Transformation DCT inverse (idct.c)

Le module de transformation en cosinus discrète inverse (IDCT) constitue l'épine dorsale mathématique du décodeur JPEG, responsable de la conversion des coefficients fréquentiels en valeurs de pixels dans le domaine spatial. 
L'implémentation propose deux approches complémentaires : une version naïve et une version optimisée. 
La version naïve (`idct_naive`) implémente fidèlement la formule mathématique de la DCT inverse bidimensionnelle sous forme de quatre boucles imbriquées, calculant pour chaque position spatiale (x,y) une somme pondérée des 64 coefficients fréquentiels, avec application des facteurs de normalisation spécifiques (`coeff_c`) pour les coefficients de fréquence nulle. 
Cette implémentation, bien que didactique et exacte, présente une complexité algorithmique de O(n⁴), ce qui la rend inadaptée aux applications exigeant des performances optimales. 
Pour pallier cette limitation, le module propose une implémentation hautement optimisée basée sur l'algorithme de Loeffler (`idct_rapide`), qui décompose la transformation 2D en une série de transformations 1D appliquées successivement aux lignes puis aux colonnes. 
Cette approche exploite le principe de séparabilité de la DCT et réduit la complexité à O(n² log n). 
L'algorithme de Loeffler subdivise chaque transformation 1D en quatre étapes distinctes minutieusement implémentées dans des fonctions dédiées : (1) un réarrangement initial des coefficients (`etape1_idct_rapide`) qui prépare les calculs ultérieurs selon un schéma non séquentiel spécifique, (2) une première série d'opérations "papillon" (`etape2_idct_rapide`) avec des rotations trigonométriques précalculées, (3) une seconde série d'opérations papillon (`etape3_idct_rapide`) intégrant des rotations plus complexes, et (4) une étape finale de réorganisation (`etape4_idct_rapide`). 
Les optimisations incluent le précalcul des coefficients trigonométriques (sinus et cosinus à différents angles comme π/16, 3π/16, π/8), la factorisation d'expressions communes dans des variables intermédiaires (`calcul`, `calcul1`, `calcul2`), et l'utilisation de tampons intermédiaires (`buff1`, `buff2`, `tmp`) pour minimiser les allocations mémoire. 
Le module gère également avec précision les aspects critiques de la transformation comme la normalisation des coefficients par la constante INV_SQRT2 (1/√2) et l'ajustement final des valeurs spatiales (multiplication par 8 et addition de 128) pour les ramener dans l'intervalle [0,255] des pixels d'image, avec saturation explicite des valeurs extrêmes. 
Cette implémentation équilibre efficacement précision numérique et performances computationnelles, constituant ainsi un composant essentiel à l'efficacité globale du décodeur.

#### Quantification inverse (iqzz.c)

Le module de quantification inverse représente une étape fondamentale dans la chaîne de décodage JPEG, responsable de la restauration de l'amplitude des coefficients DCT qui avaient été réduits lors de la compression. L'implémentation se caractérise par sa simplicité élégante et son efficacité algorithmique. 
La fonction `quantification_inverse` opère directement sur un bloc linéarisé de 64 coefficients DCT et utilise une table de quantification spécifique fournie en paramètre. 
Le principe mathématique sous-jacent est une simple multiplication terme à terme: chaque coefficient DCT est multiplié par la valeur correspondante dans la table de quantification. Cette opération inverse la division effectuée lors de la compression, restaurant ainsi progressivement les détails de l'image qui avaient été altérés par le processus de quantification. 
La structure bidimensionnelle implicite du bloc est préservée grâce à un calcul d'indice approprié (`i * N + j`), permettant d'accéder aux éléments correspondants dans le tableau linéaire. 
L'implémentation présente une complexité algorithmique optimale de O(n²), avec exactement 64 multiplications pour un bloc standard 8×8, sans opérations superflues. 
Ce module travaille en tandem avec le module de zig-zag inverse: typiquement, les coefficients sont d'abord ordonnés selon l'ordre zigzag conventionnel, puis passés à cette fonction pour la dé-quantification. 
La précision numérique est maintenue par l'utilisation du type `int` pour les coefficients, offrant une plage dynamique suffisante pour accommoder l'expansion des valeurs qui résulte de la multiplication par les facteurs de quantification, ces derniers étant codés sur 16 bits (type `uint16_t`). 
Cette approche garantit que l'information fréquentielle essentielle, particulièrement les composantes de basse fréquence cruciales pour la qualité perçue de l'image, est fidèlement restituée avant l'application de la transformation DCT inverse. 
Le module démontre une conception sobre et directe, privilégiant l'efficacité et la clarté, tout en constituant un maillon indispensable dans la reconstruction de l'image compressée.

#### Gestion des fichiers de sortie (output_file.c)

Le module de gestion des fichiers de sortie représente une composante auxiliaire mais néanmoins essentielle du décodeur JPEG, assurant la transition fluide entre les formats d'entrée et de sortie. 
L'implémentation se concentre autour de la fonction `create_output_filename` qui transforme intelligemment le chemin d'accès d'un fichier JPEG d'entrée en un nom de fichier approprié pour l'image décodée. 
Cette fonction opère selon une logique précise de manipulation de chaînes de caractères: elle localise d'abord la dernière occurrence d'un point dans le nom du fichier (indiquant l'extension) et vérifie que ce point n'appartient pas à un répertoire (en comparant sa position avec celle du dernier séparateur de chemin). 
Le traitement bifurque ensuite en fonction d'un drapeau qui indique le type d'image à produire: pour les images en couleur (flag==1), l'extension est remplacée par ".ppm" (Portable PixMap), tandis que pour les images en niveaux de gris, elle devient ".pgm" (Portable GrayMap). 
La mise en œuvre inclut une gestion rigoureuse des ressources mémoire avec l'allocation dynamique exactement dimensionnée pour accueillir le nouveau nom de fichier, et des vérifications appropriées pour éviter les fuites mémoire en cas d'échec d'allocation. 
Ce module illustre une conception pragmatique qui facilite le flux de travail du décodeur en standardisant la nomenclature des fichiers de sortie tout en préservant l'information sur leur nature (couleur ou niveaux de gris) directement dans l'extension. 
Cette approche simplifie considérablement l'utilisation du décodeur dans des pipelines de traitement d'images plus larges où l'identification automatique des formats est souvent requise.

#### Écriture des images décodées (writter.c)

Le module d'écriture des images constitue le point terminal du pipeline de décodage JPEG, responsable de la persistance et de la présentation structurée des données d'image reconstruites. 
L'implémentation s'articule autour d'une architecture à trois niveaux qui encapsule méthodiquement la gestion complète du cycle de vie des images décodées. 
La structure centrale `IMAGE_D` sert de conteneur polymorphe capable d'accueillir aussi bien des images en niveaux de gris que des images en couleur, avec une organisation mémoire optimisée selon le type d'image : pour les images en niveaux de gris, seule la composante `R` est allouée et utilisée comme canal de luminance, tandis que pour les images en couleur, les trois composantes `R`, `G` et `B` sont pleinement exploitées. 
La fonction `init_image_d` orchestre l'allocation dynamique précise des ressources mémoire adaptées aux dimensions et au format de l'image, avec une vérification rigoureuse de la validité des allocations pour garantir la robustesse du programme. 
À l'autre extrémité du cycle de vie, la fonction `free_image_d` assure une libération méticuleuse et complète des ressources allouées, prévenant ainsi les fuites mémoire potentielles en adaptant son comportement au mode de l'image (grayscale ou couleur). 
Le cœur fonctionnel du module réside dans deux opérations complémentaires : `copy_mcu_to_image` qui assure l'intégration précise des blocs 8×8 décodés (ou des MCUs de dimensions variables selon le format d'échantillonnage) dans la structure d'image globale, en tenant compte des coordonnées spatiales et en vérifiant les limites pour éviter les débordements, et `write_image_d` qui finalise le processus en générant le fichier image au format standardisé PGM (pour les images en niveaux de gris) ou PPM (pour les images en couleur), conformément aux spécifications du format Netpbm. 
Cette dernière fonction implémente avec précision la syntaxe spécifique de ces formats, incluant l'en-tête avec les dimensions et la profondeur de couleur, suivi des données de pixels arrangées séquentiellement. 
L'ensemble du module démontre une conception rigoureuse qui équilibre efficacité mémoire et clarté structurelle, offrant une interface cohérente entre le processus interne de décodage et la représentation externe standardisée des images.

#### Traitement des couleurs (traitement_couleur.c)

Le module de traitement des couleurs constitue l'orchestrateur principal du pipeline de décodage pour les images JPEG en couleur, intégrant harmonieusement les différentes étapes de transformation pour restituer l'image finale. 
L'implémentation s'articule autour de la fonction `traitement_color` qui supervise l'intégralité du processus de décompression colorimétrique en coordonnant méthodiquement les opérations sur les trois composantes Y, Cb et Cr. 
Le processus commence par une analyse fine des facteurs d'échantillonnage horizontal et vertical de chaque composante pour déterminer précisément le format de sous-échantillonnage utilisé (4:4:4, 4:2:2 horizontal, 4:2:2 vertical, ou 4:2:0), avec des vérifications de validité qui assurent la compatibilité du format avec les capacités du décodeur. 
L'architecture du module reflète la séquence complète de décodage: après la construction des tables de Huffman adaptées à chaque type de coefficient (DC/AC) et composante (luminance/chrominance), le module calcule avec précision les dimensions et le nombre de MCUs nécessaires à la reconstruction de l'image. 
Pour chaque MCU, un traitement systématique est appliqué aux blocs de coefficients DCT extraits: quantification inverse, réordonnancement zigzag inverse, puis application de la DCT inverse pour obtenir les valeurs spatiales. 
Une attention particulière est portée au traitement différencié des composantes de chrominance qui, selon le format d'échantillonnage identifié, sont soumises à des opérations spécifiques de sur-échantillonnage (up-sampling) via des fonctions adaptées (`up_sampling4_2_2_horizontal`, `up_sampling4_2_2_vertical`, ou `up_sampling4_2_0`). 
L'étape cruciale de conversion colorimétrique est ensuite réalisée par la fonction `YCbCr_to_rgb` qui transforme les valeurs YCbCr en composantes RGB conventionnelles, suivie par l'intégration des données résultantes dans la structure d'image finale via `copy_mcu_to_image`. 
L'implémentation démontre une gestion méticuleuse des ressources mémoire avec des allocations dynamiques ciblées pour chaque structure intermédiaire et des libérations systématiques après utilisation, prévenant ainsi les fuites mémoire malgré la complexité des structures imbriquées à quatre niveaux de pointeurs. 
Ce module illustre l'intégration sophistiquée des multiples algorithmes du décodeur JPEG en une séquence cohérente et efficace, tout en s'adaptant aux variations des formats d'échantillonnage qui caractérisent les différentes stratégies de compression chromatique.

#### Traitement des images en niveaux de gris (traitement_gris.c)

Le module de traitement des images en niveaux de gris constitue une implémentation épurée et optimisée du pipeline de décodage JPEG pour les images monochromes. 
L'implémentation s'articule autour de la fonction `traitement_grayscale` qui exécute séquentiellement les étapes essentielles de décompression avec une architecture simplifiée par rapport au traitement couleur. 
Le processus débute par l'initialisation des paramètres critiques et le calcul précis des dimensions de l'image en termes de MCUs (Minimum Coded Units), déterminant ainsi le nombre total de blocs 8×8 nécessaires pour reconstruire l'image complète. 
Une caractéristique distinctive de cette implémentation est sa gestion d'une unique composante de luminance (Y), éliminant ainsi la complexité inhérente aux formats d'échantillonnage chromatique. 
La séquence de décodage proprement dite commence par la génération des tables de Huffman adaptées aux composantes DC et AC à partir des spécifications extraites de l'en-tête JPEG. 
Pour chaque MCU, le module applique méthodiquement quatre transformations fondamentales : (1) le décodage du flux compressé via la fonction `decoder_bloc` qui utilise les tables Huffman pour reconstituer les 64 coefficients DCT quantifiés, (2) la quantification inverse qui restaure l'amplitude des coefficients fréquentiels, (3) l'inversion du parcours zigzag qui réorganise les coefficients en une matrice 8×8 selon leur disposition spatiale originale, et (4) l'application de la transformée DCT inverse optimisée qui convertit les coefficients fréquentiels en valeurs de pixels dans le domaine spatial. 
Chaque bloc décodé est immédiatement intégré à la structure de l'image finale via la fonction `copy_mcu_to_image`, avec une gestion méticuleuse des ressources mémoire qui libère systématiquement les allocations temporaires après chaque traitement de bloc. 
Cette approche séquentielle optimise l'empreinte mémoire du décodeur même pour des images de grande taille. 
L'implémentation se distingue par sa concision et son efficacité, privilégiant une approche directe qui élimine les étapes superflues tout en maintenant la fidélité à l'image originale, constituant ainsi une solution idéale pour les applications nécessitant un traitement rapide d'images JPEG en niveaux de gris.



