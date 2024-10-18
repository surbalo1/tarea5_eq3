#include "config.h"

int determinar_estacion(float lectura_adc[3]);

#ifdef KM


// Declaración de las funciones
float calcular_distancia_km(float lectura_adc[3], const float centroide[3]);


#endif

#ifdef KNN

#define NUM_POINTS 210  // Número de puntos en el dataset

#define K 13         // Valor de K en KNN
// Estructura para almacenar un punto
typedef struct {
    double s1, s2, s3;  // Coordenadas del punto
    int label;    // 1: invierno 2: primavera-otoño 3:verano
} Point;
void knn_init(void);
double euclidean_distance(Point a, Point b);
void find_k_nearest_neighbors(Point dataset[], Point test_point, int k, int neighbors[]);
int classify(Point dataset[], Point test_point, int k);
#endif