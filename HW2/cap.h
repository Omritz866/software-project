#ifndef CAP_H
#define CAP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITER 1000
#define DEFAULT_ITER 300

struct cord
{
    double value;
    struct cord *next;
};

struct vector
{
    struct vector *next;
    struct cord *cords;
};
double ** kmeans_clustering(double ** clusters,double ** centroids, struct vector *head_vec , int K, int D, int N, int iter, double epsilon);
#endif /* CAP_H */