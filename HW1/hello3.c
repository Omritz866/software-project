#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#define MAX_ITER 1000
#define DEFAULT_ITER 200


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
void buildClusters(double** clusters, struct vector* head_vec, int K, int D);
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
    // int *counters;
    int x;
    int iterCounter = 0;
    double distance;
    double minDistance = DBL_MAX;
    int minDistanceIndex;
    struct vector *temp; /*for freeing the memory ezer*/
    struct cord *temp1;  /*for freeing the memory ezer*/
    int **array;
    double *newCentroid;
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
            printf("Invalid maximum iteration!");
            return 1;
        }
    }
    else if (argc == 2) /* case of default iter input**/
    {
        iter = DEFAULT_ITER;
    }
    else
    { /* otherwise will fail*/
        printf("Invalid input");
        return 1;
    }
    if (K < 2 || K > N - 1)
    {
        /* checks if K valid */
        printf("Invalid number of clusters!");
        return 1;
    }

    



/*-------------------פעולה 1-----------------------*/ 
    /* Allocate memory for the 2D array*/
    clusters = (double **)malloc(K * sizeof(double *));
    i = 0;
    for (i = 0; i < K; i++)
    {
        clusters[i] = (double *)malloc((D) * sizeof(double));
    }

    buildClusters(clusters, head_vec, K, D); /* מסיים לבנות סנטרואידים*/
    return 0;
}
void buildClusters(double** clusters, struct vector* head_vec, int K, int D)
{
    /*assing with k first places*/
    struct vector* curr_vec = head_vec;
    struct cord* curr_cord = head_vec->cords;
    int i = 0;
    int j = 0;
    for (i = 0; i < K; i++) {
        for (j = 0; j < D; j++) {
            clusters[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
        curr_vec = curr_vec->next;
        curr_cord = curr_vec->cords;
    }
}