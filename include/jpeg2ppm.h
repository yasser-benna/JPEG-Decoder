// Énumération des formats d'échantillonnage pour une meilleure lisibilité
typedef enum {
    SAMPLING_444 = 0,     // Format 4:4:4 - pas de sous-échantillonnage
    SAMPLING_422_H = 1,   // Format 4:2:2 horizontal
    SAMPLING_422_V = 2,   // Format 4:2:2 vertical
    SAMPLING_420 = 3,     // Format 4:2:0
    SAMPLING_UNSUPPORTED = -1 // Format non supporté
} SamplingFormat;