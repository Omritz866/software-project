#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "cap.h"
double** createDdg(double** arr,int n,int d);//create diagonal matrik
double rowSum(double* arr,int d);//calculate sum of one row
double** createNorm(double** A,double** D,int n,int d);//create the W(normalized) matrix
void matrixMulti(double** first,double** second ,double** result ,int n,int d);//updating "result matrik" to be matrik nultiplication of first*second
double** createSim(double** X,int n,int d);//create matrix A(similarity matrix)
double getSquaredDistance(double *pointA, double *pointB, int D);/*return distance between two points*/ 
int i;
int sum;
int j;
int k;
double** createSim(double** X,int n,int d)//create matrix A(similarity matrix)
{
    double** A =(double**)malloc(n*sizeof(double*));
    for(i=0;i<n;i++)
    {
        A[i] = (double*)malloc(d*sizeof(double));
        for(j=0;j<n;j++)
        {
            if(i=j)
            {
                A[i][j] = 0;
            }
            else
            {
            A[i][j] = exp(getDistance(X[i],X[j],d)/(-2));
            }
        }
       
    }
}
double getSquaredDistance(double *pointA, double *pointB, int D) /*return distance between two points*/ 
{
    double distance = 0;
    for (j = 0; j < D; j++)
    {
        distance = distance + pow((pointA[j] - pointB[j]), 2);
    }
    return distance;
}
double** createDdg(double** arr,int n,int d)//create diagonal matrik
{
    double** D =(double**)malloc(n*sizeof(double*));
    for(i=0;i<n;i++)
    {
        D[i] = (double*)malloc(d*sizeof(double));
        D[i][i] = rowSum(arr[i]);
    }
    return D;
}
double rowSum(double* arr,int d)//calculate sum of one row
{
   sum=0;
   for(i=0;i<d;i++)
   {
      sum = sum + arr[i];
   }
   return sum;
}
double** createNorm(double** A,double** D,int n,int d)//create the W(normalized) matrik
{
    double** W =(double**)malloc(n*sizeof(double*));
    for(i=0;i<n;i++)
    {
        w[i] = (double*)malloc(d*sizeof(double));
        D[i][i] = math.pow(D[i][i],-0.5);//מנרמל את מטריצה D
    }
    matrixMulti(D,A,W,n,d);
    matrixMulti(W,D,W,n,d);
    
    return W;
}
void matrixMulti(double** first,double** second ,double** result ,int n,int d)//updating "result matrix" to be matrix nultiplication of first*second
{
for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
             for(k=0;k<n;k++)
             {
                W[i][j] += first[i][k]*second[k][j];
             }
        }
    }
}