#define PY_SSIZE_T_CLEAN
#include <Python.h> /*can ignore*/
#include <stdlib.h>
#include <math.h>
#include "cap.h"

PyObject* convert_array_to_pyobject(double** finalC, int K, int D);
static PyObject *fit(PyObject *self, PyObject *args)
{
    PyObject *centerList;
    PyObject *currFile;
    int K; /*כמות אשכולות*/
    int D; /*אורך ווקטור*/
    int N; /*כמות ווקטורים*/
    int iter;
    double epsilon;
    int Z;
    double **vectorList;
    printf ("dfshhfdhsdhdhadfhdsfhsdf");
    
    // Parse the input arguments
    if (!PyArg_ParseTuple(args, "OOiiiid", &centerList, &currFile, &K, &D, &N, &iter, &epsilon))
    {
        return NULL;
    }

    Z = PyList_Size(centerList);                                 // Get the length of the centerList
    double **clusters = (double **)malloc(Z * sizeof(double *)); // Allocate memory for the clusters array

    for (int i = 0; i < Z; i++) // Iterate over each vector in the centerList
    {
        PyObject *vector = PyList_GetItem(centerList, i);              // Get the current vector
        int vectorLength = PyList_Size(vector);                        // Get the length of the current vector
        clusters[i] = (double *)malloc(vectorLength * sizeof(double)); // Allocate memory for the current cluster
        for (int j = 0; j < vectorLength; j++)                         // Iterate over each element in the vector
        {
            PyObject *element = PyList_GetItem(vector, j); // Get the current element
            clusters[i][j] = PyFloat_AsDouble(element);    // Convert the element to a double and assign it to the cluster
        }
    }

    double **centroids = (double **)malloc(Z * sizeof(double *)); // Allocate memory for the clusters array
    for (int i = 0; i < Z; i++)                                   // Iterate over each vector in the centerList
    {
        centroids[i] = (double *)malloc(D * sizeof(double)); // Allocate memory for the current cluster
        for (int j = 0; j < D; j++)                          // Iterate over each element in the vector
        {
            centroids[i][j] = clusters[i][j]; // Convert the element to a double and assign it to the cluster
        }
    }
    print ("dfshhfdhsdhdhadfhdsfhsdf")
    // Allocate memory for the cords and vectors
    struct cord *head_cord = (struct cord *)malloc(sizeof(struct cord));
    struct cord *curr_cord = head_cord;
    curr_cord->next = NULL;

    struct vector *head_vec = (struct vector *)malloc(sizeof(struct vector));
    struct vector *curr_vec = head_vec;
    curr_vec->next = NULL;

    for (int i = 0; i < N; i++)
    {                                                   // Iterate over each vector in the currFile
        PyObject *vector = PyList_GetItem(currFile, i); // Get the current vector
        int vectorLength = PyList_Size(vector);         // Get the length of the current vector

        vectorList[i] = (double *)malloc(vectorLength * sizeof(double)); // Allocate memory for the current cluster
        for (int j = 0; j < vectorLength; j++)
        {                                                  // Iterate over each element in the vector
            PyObject *element = PyList_GetItem(vector, j); // Get the current element
            vectorList[i][j] = PyFloat_AsDouble(element);  // Convert the element to a double and assign it to the cluster
            curr_cord->value = vectorList[i][j];           // Assign the current value to the cord
            curr_cord->next = malloc(sizeof(struct cord)); // Move to the next cord
            curr_cord = curr_cord->next;
            curr_cord->next = NULL;

            // Check if it's the first vector to update D
            if (i == 0)
            {
                D++;
            }
        }

        // Set the cords of the current vector
        curr_vec->cords = head_cord;

        // Move to the next vector
        curr_vec->next = malloc(sizeof(struct vector));
        curr_vec = curr_vec->next;
        curr_vec->next = NULL;

        // Reset the head_cord for the next vector
        head_cord = malloc(sizeof(struct cord));
        curr_cord = head_cord;
        curr_cord->next = NULL;
    }
    
    if(kmeans_clustering(clusters,centroids, head_vec , K, D,N,iter,epsilon) == NULL) {
        /*yErr_SetString(PyExc_RuntimeError, GENERAL_ERROR);*/
        return NULL;
    }

    PyObject* py_centroids = convert_array_to_pyobject(centroids, K, D);
    int i;
    for (i = 0; i < K; i++) 
    {
        free(centroids[i]);
    }
    free(centroids);


    return py_centroids;
    
}
PyObject* convert_array_to_pyobject(double** finalC, int K, int D) 
{
    PyObject* outer_list = PyList_New(K);
    int i;
    int j;
    for( i = 0; i < K; i++) 
    {
        PyObject* inner_list = PyList_New(D);
        for( j = 0; j < D; j++) 
        {
            PyObject* py_double = PyFloat_FromDouble(finalC[i][j]);
            PyList_SetItem(inner_list, j, py_double);  // Note: PyList_SetItem steals a reference to item.
        }
        PyList_SetItem(outer_list, i, inner_list);
    }

    return outer_list;
}

/* Create wrapper functions for the fit function */
/* &centerList, &currFile, &K, &D, &N, &iter, &epsilon*/
static PyMethodDef fit_methods[] = {
        {"fit", /* the Python method name that will be used */
                (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
                     METH_VARARGS, /* flags indicating parameters accepted for this function */
                PyDoc_STR("The fit function receives as input the following arguments:"
                          "centerList - a Python list representing the initial centroids as chosen by the kmeans++"
                          "currFile - a Python list representing all the data points for k means clustering"
                          "k - an Python Integer representing the k in k means clustering"
                          "D - an Python Integer representing the dimension of each data point in k means clustering"
                          "N - an Python Integer representing the number of vectors in the voctorsList"
                          "iter - an Python Integer representing the max number of iterations in k means clustering"
                          "eps - a Python float representing the epsilon in k means clustering"
                          "algorithm."
                          "this function will then perform k means clustering on the inputted parameters and will return the"
                          "final centroids as a Python list.")}, /*  The docstring for the function */
        {NULL, NULL, 0, NULL} /* The last entry must be all NULL as shown to act as a sentinel. Python looks for this
         * entry to know that all the functions for the module have been defined. */
};

static struct PyModuleDef kmeansmodule = {
        PyModuleDef_HEAD_INIT,
        "kmeanssp", /* name of module */
        NULL, /* module documentation, may be NULL */
        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
        fit_methods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_kmeanssp(void) {
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m) {
        return NULL;
    }
    return m;
}
