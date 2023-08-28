#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#define MAX_ITER 1000
#define DEFAULT_ITER 200
#define EPSILON 0.001
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
void buildClusters(double **clusters, struct vector *head_vec, int K, int D);
void buildCentroids(double **centroids, struct vector *head_vec, int K, int D);
double getDistance1(double *centroids, struct vector *head_vec, int D);
double getDistance2(double *centroids, double *newCentroids, int D);
void printClusters(double **, int K, int D);
double *calculateCentroids(double *newCentroid,int **arr, struct vector *head_vec, int *counters, int D, int N, int i);
int updateCentroids(double **centroids, int **arr, struct vector *head_vec, int *counters, int K, int D, int N);
int main(int argc, char **argv)
{
    struct vector *head_vec;
    struct vector *curr_vec;
    struct cord *head_cord;
    struct cord *curr_cord;
    double n;  /*to reading the file*/
    char c;    /*to reading the file*/
    int N = 0; /*counts the rows*/
    int D = 1; /* counts the columns */
    int K = atoi(argv[1]);
    int iter;
    double **clusters;
    int i;
    int j;
    int *counters;
    int iterCounter = 0;
    double distance;
    double minDistance = DBL_MAX;
    int minDistanceIndex;
    struct vector *temp; /*for freeing the memory ezer*/
    struct cord *temp1;  /*for freeing the memory ezer*/

    int **array;
    int epsilonIndicator = 0;

    FILE *file = stdin;
    head_cord = malloc(sizeof(struct cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;
    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;

    while (fscanf(file, "%lf%c", &n, &c) == 2)
    /* builds the 2d array by reading the file */
    {
        if (c == '\n')
        {
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            /* brings back the curr_vec to the first cord */
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
            N++;
            continue;
        }

        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(struct cord));
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
        if (N == 0)
        {
            D++;
        }
    }

    /* finished reading the file */

    /* checking variables conditions */
    if (argc == 3) /*in cases of iter input */
    {
        iter = atoi(argv[2]);
        if (iter < 2 || iter > MAX_ITER - 1)
        {
            /* checks if iter valid */
            printf("Invalid maximum iteration!\n");
            return 1;
        }
    }
    else if (argc == 2) /* case of default iter input**/
    {
        iter = DEFAULT_ITER;
    }
    else
    { /* otherwise will fail*/
        printf("Invalid input\n");
        return 1;
    }
    if (K < 2 || K > N - 1)
    {
        /* checks if K valid */
        printf("Invalid number of clusters!\n");
        return 1;
    }

    clusters = (double **)malloc(K * sizeof(double *));
    i = 0;
    for (i = 0; i < K; i++)
    {
        clusters[i] = (double *)malloc((D) * sizeof(double));
    }

    buildClusters(clusters, head_vec, K, D); /* מסיים לבנות סנטרואידים*/

    counters = malloc(K * sizeof(int)); /* יוצרים מערך מונים ומאתחלים אותו ל0 כל אחד*/
    for (i = 0; i < K; i++)
    {
        counters[i] = 1;
    }

    array = (int **)malloc(K * sizeof(int *)); /*מאתחל מערך בינארי שישמש אידיקציה לשייכות אשכול*/
    for (i = 0; i < K; i++)
    {
        array[i] = (int *)malloc(N * sizeof(int));
    }

    while (epsilonIndicator == 0 && iterCounter < iter)
    {
        curr_vec = head_vec; /*before entereing the loop we set currs vec and cord to top*/
        curr_cord = curr_vec->cords;
        for (i = 0; i < K; i++) /*updating boolean array to zero for all iteration*/
        {
            for (j = 0; j < N; j++)
            {
                array[i][j] = 0;
            }
        }

        for (i = 0; i < K; i++)
        {
            counters[i] = 0;
        }
        for (i = 0; i < N; i++) /*this loop finds the the minimum distance and relevant cluster*/
        {
            minDistance = DBL_MAX;
            minDistanceIndex = -1;
            for (j = 0; j < K; j++) /*finds the attachment cluster */
            {
                distance = getDistance1(clusters[j], curr_vec, D);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    minDistanceIndex = j;
                }
            }

            counters[minDistanceIndex]++;
            array[minDistanceIndex][i] = 1;
            curr_vec = curr_vec->next;
            curr_cord = curr_vec->cords;
        }

        iterCounter++;
        epsilonIndicator = updateCentroids(clusters, array, head_vec, counters, K, D, N);
    }
    printClusters(clusters, K, D);

    /* Free the allocated memory of given array */
    curr_vec = head_vec;
    while (curr_vec != NULL)
    {
        curr_cord = curr_vec->cords;
        while (curr_cord != NULL)
        {
            temp1 = curr_cord;
            curr_cord = curr_cord->next;
            free(temp1);
        }
        temp = curr_vec;
        curr_vec = curr_vec->next;
        free(temp);
    }

    /* Free the allocated memory of our clusters*/
    for (i = 0; i < K; i++)
    {
        free(clusters[i]);
    }
    free(clusters);

    for (i = 0; i < K; i++)/*free array*/
    {
        free(array[i]);
    }
    free(array);

    free(counters); /*free counters*/
    free(head_cord);
    return 0;
}
void buildClusters(double **clusters, struct vector *head_vec, int K, int D)
{
    /*assing with k first places*/
    struct vector *curr_vec = head_vec;
    struct cord *curr_cord = head_vec->cords;
    int i = 0;
    int j = 0;
    for (i = 0; i < K; i++)
    {
        for (j = 0; j < D; j++)
        {
            clusters[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
        curr_vec = curr_vec->next;
        curr_cord = curr_vec->cords;
    }
}
void buildcentroids(double **centroids, struct vector *head_vec, int K, int D)
{
    /* assing with k first places */
    struct vector *curr_vec = head_vec;
    struct cord *curr_cord = head_vec->cords;
    int i = 0;
    int j = 0;
    for (i = 0; i < K; i++)
    {
        for (j = 0; j < D; j++)
        {
            centroids[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
        curr_vec = curr_vec->next;
        curr_cord = curr_vec->cords;
    }
}
double *calculateCentroids(double *newCentroid,int **array, struct vector *head_vec, int *counters, int D, int N, int lineIndex)
{
    struct vector *curr_vec = head_vec;
    struct cord *curr_cord = head_vec->cords;
    int h = 0;
    int j;
    int counter;
    for (h = 0; h < D; h++)
    {
        newCentroid[h] = 0;
    }
    for (j = 0; j < N; j++) /*checks if in arrays which vectors the cluster contains*/
    {
        counter = 0;
        if (array[lineIndex][j] == 1)
        {
            counter += 1;
            for (h = 0; h < D; h++)
            {
                newCentroid[h] = newCentroid[h] + curr_cord->value;
                curr_cord = curr_cord->next;
            }
        }
        curr_vec = curr_vec->next;
        curr_cord = curr_vec->cords;
    }
    for (h = 0; h < D; h++)
    {
        newCentroid[h] = newCentroid[h] / (counters[lineIndex]);
    }
    
    return newCentroid;
}
int updateCentroids(double **centroids, int **array, struct vector *head_vec, int *counters, int K, int D, int N)
{
    double *newCentroid = (double *)malloc(D * sizeof(double));
    int epsilonIndicator = 1;
    int h = 0;
    int i = 0;
    for (i = 0; i < K; i++) /*run on the k clusters* and checks if the new centroid - curr centroid for all is below epsilon*/
    {
        newCentroid = calculateCentroids(newCentroid,array, head_vec, counters, D, N, i);
        if (getDistance2(centroids[i], newCentroid, D) >= EPSILON)
        {
            epsilonIndicator = 0;
        }
        for (h = 0; h < D; h++)
        {
            centroids[i][h] = newCentroid[h];
        }
    }
    free (newCentroid);
    return epsilonIndicator;
}
double getDistance2(double *centroids, double *newCentroids, int D) /*return distance between vector and array*/ 
{
    int j = 0;
    double distance = 0;
    for (j = 0; j < D; j++)
    {
        distance = distance + pow((centroids[j] - newCentroids[j]), 2);
    }
    distance = sqrt(distance);
    return distance;
}
double getDistance1(double *centroids, struct vector *head_vec, int D) /* return distance between vector and array */
{
    int j = 0;
    double distance = 0;
    struct cord *curr_cord = head_vec->cords;

    for (j = 0; j < D; j++)
    {
        distance = distance + pow((centroids[j] - curr_cord->value), 2);
        curr_cord = curr_cord->next;
    }
    distance = sqrt(distance);
    return distance;
}
void printClusters(double **clusters, int K, int D)
{
    /* Print clusters*/
    int i = 0;
    int j = 0;
    for (i = 0; i < K; i++)
    {
        for (j = 0; j < D; j++)
        {
            /* Print a comma before every value except the first one in each line*/
            if (j != 0) {
                printf(",");
            }
            printf("%.4f", (clusters[i][j]));
        }
        printf("\n");
    }
}