#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*Global constants declarations*/
const double EPSILON = 0.001;
const int DEFAULT_ITER = 200;
const int MAX_ITER_LIMIT = 1000;
const char *GENERAL_ERROR = "An Error Has Occurred";

/* Define the struct for a node in the list. */
typedef struct Node {
    double *value;            /* The 'value' in the node. */
    struct Node *next;      /* Pointer to the next node. */
    struct Node *previous;
} Node;

/* Define the struct for the linked list. */
typedef struct LinkedList {
    Node *head; /* The head of the list. */
    int node_counter;
} LinkedList;

/* Function declarations*/
double **k_means_clustering(int, int, int, LinkedList *);
int count_dimensions(const char *);
LinkedList* get_data_points_list(char *, int, int*);
double **copy_first_k_elements(LinkedList *, int, int);
int get_closest_centroid_index(double **, Node *, int, int);
void update_new_centroid(double *, LinkedList *, int);
void update_counter(double *, double *, int *, int);
void print_centroids(double **, int, int);
void free_centroids(double**, int);
void reassign_centroid(LinkedList *, LinkedList *, Node *);
double calculate_distance(const double *, const double *, int);
void split_string_to_doubles(char* str, double* values, int dim);
LinkedList* parse_data(int*, int*);
char* read_line();
LinkedList *create_list();
LinkedList* create_list_array(int);
Node *create_node(double *);
int add_new_node(LinkedList *, double *);
void remove_node(LinkedList *, Node *);
void add_existing_node(LinkedList *, Node *);
void delete_list_nodes(LinkedList *);


/* Function to create a new list.
 * Allocates memory for a LinkedList struct and initializes its head to NULL.
 */
LinkedList *create_list_array(int k) {
    int i;

    LinkedList *list_ptr = (LinkedList *)malloc(k * sizeof(LinkedList));
    if (list_ptr == NULL) {
        return NULL;
    }
    for(i=0; i<k; i++){
        list_ptr[i].head = NULL;
        list_ptr[i].node_counter = 0;
    }
    return list_ptr;
}

LinkedList *create_list() {
    LinkedList *list_ptr = (LinkedList *)malloc(sizeof(LinkedList));
    if (list_ptr == NULL) {
        printf("%s", GENERAL_ERROR);
        exit(1);
    }
    list_ptr->head = NULL;
    list_ptr->node_counter = 0;
    return list_ptr;
}

/* Function to create a new node.
 * Allocates memory for a Node struct, initializes its value to the passed double pointer,
 * and its next and previous pointers to NULL.
 */
Node *create_node(double *value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->value = value;
    new_node->next = NULL;
    new_node->previous = NULL;
    return new_node;
}

/* Function to add a new node to the list.
 * Creates a new node and adds it to the list using the add_existing_node function.
 */
int add_new_node(LinkedList *list, double *value) {
    Node *new_node = create_node(value);
    if(new_node == NULL)
        return 0;
    add_existing_node(list, new_node);
    return 1;
}

/* Function to remove a node from the list
 * Updates pointers to remove the node from the list. Does not free the node's memory.
 */
void remove_node(LinkedList *list, Node *node) {
    if(node == NULL || list->head == NULL) {
        return;
    }
    if (node->previous == NULL) {
        list->head = node->next;
    } else {
        node->previous->next = node->next;
    }
    if (node->next != NULL) {
        node->next->previous = node->previous;
    }
    node->next = NULL;
    node->previous = NULL;
    list->node_counter--;
}

/* Function to add an existing node to a list
 * Adjusts pointers to insert the node at the head of the list.
 */
void add_existing_node(LinkedList *list, Node *node) {
    if(list == NULL || node == NULL) {
        return;
    }

    node->next = list->head; /* Point the new node's next to the old head*/
    node->previous = NULL;   /* The new node is now the first node, so its previous is NULL*/

    if (list->head != NULL) {
        list->head->previous = node; /* Point the old head's previous to the new node*/
    }

    list->head = node; /* Point the head of the list to the new node*/
    list->node_counter++;
}

/* Function to delete the entire list.
 * This function must be called on lists with a head that was not freed. The head can be null or a valid node, but not
 * trash.
 */
void delete_list_nodes(LinkedList *list) {
    Node *current = list->head;
    while(current != NULL) {
        list->head = current->next;
        free(current->value);
        free(current);
        current = list->head;
    }
}

/**
 * Function to perform k-means clustering.
 * Repeatedly assigns data points to the closest centroid, then updates the centroids to the mean of their clusters.
 * Stops when the centroids have converged, or the maximum number of iterations is reached.
 * @param k The number of clusters to form.
 * @param iter The maximum number of iterations to perform.
 * @param input_data The path to the input data file.
 * @return An array containing the final centroids after performing k-means clustering.
 */
double **k_means_clustering(int k, int dim, int iter, LinkedList *data_points_ptr) {
    double **centroids_arr = copy_first_k_elements(data_points_ptr, dim, k); /* initializes the first iteration of centroids_arr*/
    int epsilon_counter, index, i1, i2, i3, i4;
    double* new_centroid = (double *) malloc(dim *sizeof(double));
    Node * current;
    LinkedList * clusters;

    if (new_centroid == NULL) {
        printf("%s", GENERAL_ERROR);
        delete_list_nodes(data_points_ptr);
        free(data_points_ptr);
        free(centroids_arr);
        exit(1);
    }

    /* this creates an array of k empty lists. NOTE - WE MUST FREE THIS MEMORY EVENTUALLY*/

    clusters = create_list_array(k);
    if(clusters == NULL){
        printf("%s", GENERAL_ERROR);
        delete_list_nodes(data_points_ptr);
        free(data_points_ptr);
        free_centroids(centroids_arr, k);
        free(new_centroid);
        exit(1);
    }
    current= data_points_ptr->head;

    /* Initialize initial clusters array, each cell of the array will contain a list, and each node of the list is of
     a data point array.
     For each data point, find the closest centroid and assign the data point to its cluster
     after this while loop we have an array of lists, each one corresponding to the centroid of the same index
    */
    while (1) {
        /* Get the index of the closest centroid (each cluster and centroid share the same index).*/
        index = get_closest_centroid_index(centroids_arr, current, dim, k);
        if(current->next != NULL) {
            current = current->next;
            add_existing_node(&clusters[index], current->previous); /* Add the current data point to its relevant cluster.*/
        }
        else{
            add_existing_node(&clusters[index], current);
            break;
        }
    }

    for (i1 = 0; i1 < iter; i1++) { /* iterate over maximum number of iterations.*/
        epsilon_counter = 0; /* Counter to track how many centroids_arr have converged.*/
        /* If this is not the first iteration: for each data point, if it has a new closest centroid, then reassign it.*/
        if (i1 >= 1) {
            for(i2 = 0; i2<k; i2++){
                current = clusters[i2].head;
                while(current != NULL){
                    index = get_closest_centroid_index(centroids_arr, current, dim, k);

                    if(index != i2){
                        if(current->next != NULL) {
                            current = current->next;
                            /* removes current from clusters[i2] and adds it to clusters[index]*/
                            reassign_centroid(&clusters[i2], &clusters[index], current->previous); /* old cluster pointer, new cluster pointer , node pointer*/
                        }
                        else{
                            reassign_centroid(&clusters[i2], &clusters[index], current);
                            break;
                        }
                    }
                    else{
                        current = current->next;
                    }
                }
            }
        }
        /* for each cluster, calculate the mean of the points (the new centroid) and update its positions*/
        for (i2 = 0; i2 < k; i2++) {
            for(i3=0; i3<dim; i3++) {
                new_centroid[i3]=0;
            }

            update_new_centroid(new_centroid, &clusters[i2], dim); /* new_centroid will contain the new centroid of the cluster*/

            update_counter(new_centroid, centroids_arr[i2], &epsilon_counter, dim); /* adds 1 to epsilon_counter if change is smaller than EPSILON*/

            for(i4 = 0; i4 < dim; i4++) {
                centroids_arr[i2][i4] = (double) new_centroid[i4];
            }

        }

        /* If all centroids_arr have converged, stop the iteration and return the final centroid positions.*/
        if (epsilon_counter == k) {
            break;
        }
    }
    for(i1=k-1; i1>=0;i1--){
        delete_list_nodes(&clusters[i1]);
    }
    free(clusters);
    data_points_ptr->head = NULL;
    free(data_points_ptr);
    free(new_centroid);
    return centroids_arr;
}


/**
 * @pre file_name is a valid path to a file, and the content of the file is in the required format.
 * Function to count dimensions.
 * Counts the number of commas in the passed string, and returns this count plus one.
 * Assumes that the string represents a vector, with dimensions separated by commas.
 * @param file_name A String containing the path of the relevant file.
 * @return The number of dimensions of each data point (All data points share the same number of dimensions).
 */
int count_dimensions(const char * vector) {
    int count = 1;
    int i;
    for (i = 0; vector[i] != '\0'; i++) {
        if (vector[i] == ',') {
            count++;
        }
    }
    return count;
}


/**
 * @pre file_name is a valid path to a file, and the content of the file is in the required format.
 * Function to parse data points from input and build a linked list, in reverse order of the input.
 * The function reads the input line by line, splits each line at commas,
 * and adds each resulting array of values to the list.
 * count_lines_pointer is updated to reflect the total number of lines read.
 * @param input_data A String containing the path of the relevant file.
 * @param dim The number of dimensions of each data point.
 * @return A linked list, where each node is a data point array of size dim.
 */
LinkedList* get_data_points_list(char * line, int dim, int* count_lines_pointer) {
    LinkedList *list = create_list();
    int line_counter = 0;
    double* values = malloc(dim * sizeof(double));
    int c;
    char buffer[24];
    int buffer_index = 0;
    int i;


    if (values == NULL) {
        printf("%s", GENERAL_ERROR);
        free(list); /* no nodes in list, so it's fine to free it directly*/
        free(line);
        exit(1);
    }

    split_string_to_doubles(line, values, dim);
    if(add_new_node(list, values) == 0){
        printf("%s",GENERAL_ERROR);
        free(list);
        free(values);
        exit(1);
    }
    line_counter++;
    free(line);

    i = 0;
    values = malloc(dim * sizeof(double));
    if (values == NULL) {
        printf("%s", GENERAL_ERROR);
        delete_list_nodes(list);
        free(list);
        exit(1);
    }

    while ((c = getchar()) != EOF) {
        if (c != ',' && c != '\n') {
            buffer[buffer_index++] =(char) c;
        } else {
            buffer[buffer_index] = '\0';
            values[i++] = strtod(buffer, NULL);
            buffer_index = 0;
            if (c == '\n') {
                if(add_new_node(list, values) == 0){
                    printf("%s", GENERAL_ERROR);
                    delete_list_nodes(list);
                    free(list);
                    free(values);
                    exit(1);
                }
                line_counter++;
                i = 0;

                values = malloc(dim * sizeof(double));
                if (values == NULL) {
                    printf("%s", GENERAL_ERROR);
                    delete_list_nodes(list);
                    free(list);
                    exit(1);
                }
            }
        }
    }
    free(values);
    *count_lines_pointer = line_counter;
    return list;
}

/*
 * This function splits a given string by commas and converts the substrings to double values.
 * The function assumes the provided string contains numerical values separated by commas.
 * It iterates over the string character by character, splitting it at commas,
 * and then converts each substring to a double value.
 * These values are then stored in the provided 'values' array.
 */
void split_string_to_doubles(char* str, double* values, int dim) {
    int index;
    int i;
    char* current_double_pointer;
    char delim;

    delim = ',';

    index = 0;
    current_double_pointer = str;
    for (i = 0; i < dim; i++) {
        while (str[index] != delim && str[index] != '\0') {
            index++;
        }
        str[index] = '\0';
        values[i] = strtod(current_double_pointer, NULL);

        current_double_pointer = &str[index + 1];
        index++;
    }
}

/**
 *  Function to copy the first k elements of the linked list to a 2D array.
 * The 2D array is allocated dynamically and needs to be freed later.
 * The elements are copied in reverse order, so the first element of the list becomes the last element of the array.
 * @param data_points A linked list, where each node is a data point array of size dim.
 * @param dim The number of dimensions of each data point.
 * @return An array of size k where each cell is a data point array of size dim, with the data points being the first k
 * data points (nodes) in the inputted linked list.
 */
double **copy_first_k_elements(LinkedList *data_points, int dim, int k) {
    /* Allocate memory for the outer array*/
    Node * current;
    int i, j;
    double **centroids = (double **) malloc(k * sizeof(double *));
    if (centroids == NULL) {
        printf("%s", GENERAL_ERROR);
        delete_list_nodes(data_points);
        free(data_points);
        exit(1);
    }

    current = data_points->head;
    while(current->next != NULL){
        current = current->next;
    }
    for (i = 0; i < k; i++) {
        /* Allocate memory for the inner array*/
        centroids[i] = (double *) malloc(dim * sizeof(double));
        if (centroids[i] == NULL) {
            printf("%s", GENERAL_ERROR);
            delete_list_nodes(data_points);
            free(data_points);
            free_centroids(centroids, i);
            exit(1);
        }

        /* Copy the values*/
        for (j = 0; j < dim; j++) {
            centroids[i][j] = current->value[j];
        }
        current = current->previous;
    }
    return centroids;
}


/**
* Function to find the index of the centroid closest to a given data point.
* The distance from each centroid to the data point is calculated,
* and the index of the centroid with the minimum distance is returned.
* @param centroids: A pointer to an array of pointers to double. Each inner pointer represents a centroid in the dataset.
* @param data_point: A pointer to the Node structure that represents the data point for which we want to find the closest centroid.
* @param dim: An integer representing the number of dimensions in the dataset.
* @param k: An integer representing the number of centroids.
* @return: Returns the index of the closest centroid to the given data point.
*/
int get_closest_centroid_index(double **centroids, Node *data_point, int dim, int k) {
    int i, min_distance_index;
    double distance, min_distance = INFINITY;
    /* Loop through all the centroids*/
    for (i = 0; i < k; i++) {
        /*Calculate the distance between the current centroid and the data point*/
        distance = calculate_distance(centroids[i], data_point->value, dim);
        /*If the calculated distance is less than the current minimum distance*/
        if (distance < min_distance) {
            /* Update the minimum distance and the index of the centroid with the minimum distance*/
            min_distance_index = i;
            min_distance = distance;
        }
    }
    /* Return the index of the centroid with the minimum distance to the data point*/
    return min_distance_index;
}

/**
 * * Function to calculate the Euclidean distance between two points.
 * The points are represented as arrays of doubles, and their dimension is given by dim.
 * @param centroid: A pointer to a double array that represents a centroid in the dataset.
 * @param data_point: A Node structure that represents a data point in the dataset.
 * @param dim: An integer representing the number of dimensions in the dataset.
 * @return: Returns the Euclidean distance between the centroid and the data point.
 */
double calculate_distance(const double *centroid, const double *data_point, int dim){
    long double sum = 0.00;
    int i;
    /*Loop through all the dimensions*/
    for(i = 0; i < dim; i++){
        /* Calculate the square of the difference between the current dimension of the centroid and the data point*/
        sum += ((centroid[i] - data_point[i])*(centroid[i] - data_point[i]));
    }
    /* Return the square root of the sum, which is the Euclidean distance*/
    sum = sqrtl(sum);
    return (double) sum;
}

/**
 * Function to update the values of a centroid.
 * The new values are the averages of the corresponding values of all the data points in a cluster.
 * @param new_centroid The array that will contain the new centroid.
 * @param cluster The cluster of which we will calculate a new centroid for.
 */
void update_new_centroid(double *new_centroid, LinkedList *cluster, int dim) {
    int dim_iterator;
    Node * current = cluster->head;

    while(current != NULL) {
        for(dim_iterator =0; dim_iterator < dim; dim_iterator++ ) {
            new_centroid[dim_iterator] += (*(current->value + dim_iterator)/cluster->node_counter);
        }
        current = current->next;
    }
}


/**
 * Function to check if the change in centroid position is less than a threshold (EPSILON).
 * If the change is less than EPSILON, epsilon_counter is incremented.
 * @param new_centroid The new centroid of a certain cluster
 * @param old_centroid The previous centroid of a certain cluster
 * @param epsilon_counter A pointer to epsilon_counter from the k_means_clustering function.
 */
void update_counter(double *new_centroid, double *old_centroid, int *epsilon_counter, int dim) {
    if(calculate_distance(old_centroid, new_centroid, dim) < EPSILON)
        (*epsilon_counter)++;
}


/**
 * Function to print the centroids.
 * Each centroid is represented as an array of doubles.
 * Each centroid is printed on a new line, with its values separated by commas.
 */
void print_centroids(double **centroids, int dim, int k) {
    int i,j;
    for(i = 0; i < k; i++) {
        for(j = 0; j < dim; j++) {
            /* Print a comma before every value except the first one in each line*/
            if (j != 0) {
                printf(",");
            }
            printf("%.4f", centroids[i][j]);
        }
        printf("\n");
    }
}

/**
* Function to reassign a data point to a new cluster.
* The data point is represented as a node in the linked list of its current cluster.
* The node is removed from the list of the old cluster and added to the list of the new cluster.
*/
void reassign_centroid(LinkedList * old_cluster_pointer, LinkedList * new_cluster_pointer, Node * node_pointer){
    remove_node((old_cluster_pointer),(node_pointer));
    add_existing_node((new_cluster_pointer),(node_pointer));
}
/** Function to free the memory allocated for the centroids.
 * The centroids are represented as a 2D array of doubles.
*/
void free_centroids(double **centroids, int k) {
    int i;
    for (i = 0; i < k; i++) {
        free(centroids[i]);
    }
    free(centroids);
}
/**
* Function to parse the data input.
* The first line is read as a whole and then used to determine the dimensions of input vectors.
* Then the first line, along with the rest of STDIN is saved as a list of vectors.
*/

LinkedList* parse_data(int* dim_ptr, int* count_lines_ptr){
    LinkedList * list;
    char* line = read_line();
    *dim_ptr = count_dimensions(line);
    list = get_data_points_list(line, *dim_ptr, count_lines_ptr);
    return list;
}
/**
* Function to read a line from standard input.
* The line is allocated dynamically and needs to be freed later.
*/
char *read_line() {
    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t result = getline(&line, &buffer_size, stdin);
    if(result != -1)
        return line;
    printf("%s", GENERAL_ERROR);
    exit(1);
}

/**
 * The main function of the program. It takes command-line arguments, validates them, performs the K-means clustering,
 * and then prints the centroids.
 *
 * @return Exit status of the program.
 */
int main(int argc, char** argv) {
    int program_terminator = 0; /* Bool to identify issues with the input that should result in exiting the program*/
    int k;
    int max_iterations;
    LinkedList* data_points_ptr;
    double** centroids;
    int dim;
    int line_count;
    /* Extract the number of clusters and maximum number of iterations from the command-line arguments*/
    k = strtol(argv[1], NULL, 10);

    /* If only two arguments are given, use the default number of iterations*/
    if(argc == 2){
        max_iterations = DEFAULT_ITER;
    }
        /* If three arguments are given, use the second argument as the maximum number of iterations*/
    else {
        max_iterations = strtol(argv[2], NULL, 10);
        if(max_iterations <= 1 || max_iterations >= 1000){
            printf("Invalid maximum iteration!");
            program_terminator = 1;
        }
    }

    data_points_ptr = parse_data(&dim, &line_count);
    /* Validate the number of clusters. It should be greater than 1 and less than the number of data points.*/
    if (k <= 1 || k > line_count) {
        printf("Invalid number of clusters!");
        program_terminator = 1;
    }

    /* Validate the maximum number of iterations. It should be within the range [1, max_iter_limit].*/
    if (max_iterations < 1 || max_iterations > MAX_ITER_LIMIT) {
        printf("Invalid maximum iteration!");
        program_terminator = 1;
    }

    /* If either the number of clusters or the maximum number of iterations is invalid, terminate the program.*/
    if (program_terminator) {
        delete_list_nodes(data_points_ptr);
        free(data_points_ptr);
        exit(1);
    }

    /* Perform K-means clustering and store the results in the centroids array*/
    centroids = k_means_clustering(k, dim, max_iterations, data_points_ptr);

    /* Print the final centroids */
    print_centroids(centroids, dim, k);

    /* Free the memory allocated for the centroids*/
    free_centroids(centroids, k);
    return 0; /* Successful termination of the program*/
}