import math
import sys

EPSILON = 0.001  # Defines the precision for determining the convergence of centroids.
DEFAULT_ITER = 200  # Default number of iterations if none is specified by the user.
MAX_ITER_LIMIT = 1000  # Maximum limit on the number of iterations allowed.


# This function performs the K-means clustering algorithm on a given set of data.
# k: the number of clusters to form.
# iter: the maximum number of iterations to perform.
# input_data: the path to the input data file.
def find_centroids(k, max_iter, input_data):
    # Load the data points from the input file.
    data_points = get_data_points_list(input_data)
    # Initialize centroids using the first k data points.
    centroids = data_points[:k]
    # Initialize an empty cluster list for each centroid.
    clusters = [[] for _ in range(k)]

    # Iterate over the maximum number of iterations.
    for iteration_number in range(max_iter):
        epsilon_counter = 0  # Counter to track how many centroids have converged.

        # For each data point, find the closest centroid and assign the data point to that cluster.
        for data_point in data_points:
            i = get_closest_centroid_index(centroids, data_point)
            clusters[i].append(data_point)

        # For each cluster, calculate the mean of the points (the new centroid) and update its position.
        for i, cluster in enumerate(clusters):
            if len(cluster) > 0:
                # new_centroid = [round(sum(dim) / len(cluster), 4) for dim in zip(*cluster)]
                new_centroid = [round(dim / len(cluster), 4) for dim in map(sum, zip(*cluster))]
                if calculate_distance(centroids[i], new_centroid) < EPSILON:
                    epsilon_counter += 1
                centroids[i] = new_centroid
            else:
                epsilon_counter += 1;
        # If this is not the last iteration, reset the clusters for the next round.
        if iteration_number < max_iter - 1:
            clusters = [[] for _ in range(k)]

        # If all centroids have converged, stop the iteration and return the final centroid positions.
        if epsilon_counter == k:
            return centroids

    # If reached the maximum number of iterations without all centroids converging, return current centroids.
    return centroids


def calculate_distance(vector1, vector2):
    sum_squares = 0
    for i in range(len(vector1)):
        sum_squares += (vector1[i] - vector2[i]) ** 2
    distance = sum_squares ** 0.5
    return distance


# This function finds the index of the closest centroid to a given data point.
# It calculates the euclidean distance from the data point to each centroid and returns the index of the closest.
def get_closest_centroid_index(centroids, data_point):
    distances = [calculate_distance(data_point, centroid) for centroid in centroids]
    return distances.index(min(distances))


# This function reads the data file line by line, splits each line by comma, converts the elements into floats,
# and appends the result as a list of floats to the data_points list.
def get_data_points_list(input_data):
    with open(input_data, "r") as file:
        data_points = [list(map(float, line.split(','))) for line in file]
    return data_points


# This function takes a list of centroids and prints them in a formatted way.
# Each centroid is printed on a new line, and the coordinates of the centroid are separated by commas.
def print_centroids(centroids):
    my_string = ''
    for centroid in centroids:
        my_string += ','.join(map(str, centroid)) + '\n'
    print(my_string)


# This function counts the number of lines in a given file.
def count_lines(filename):
    with open(filename, 'r') as file:
        return sum(1 for _line in file)


# The main function of the program. It takes command-line arguments, validates them, performs
# the K-means clustering, and then prints the centroids.
def main():
    program_terminator = False
    k = int(sys.argv[1])  # Number of clusters.
    input_data = sys.argv[len(sys.argv) - 1]  # The path of the input file.

    # If only two arguments are given, use the default number of iterations.
    # If three arguments are given, use the second argument as the maximum number of iterations.
    max_iterations = DEFAULT_ITER if len(sys.argv) == 3 else int(sys.argv[2])

    # Validate the number of clusters. It should be greater than 1 and less than the number of data points.
    if k <= 1 or k > count_lines(input_data) - 1:
        print('Invalid number of clusters!')
        program_terminator = True

    # Validate the maximum number of iterations. It should be within the range [1, max_iter_limit].
    if max_iterations < 1 or max_iterations > MAX_ITER_LIMIT:
        print('Invalid maximum iteration!')
        program_terminator = True

    # If either the number of clusters or the maximum number of iterations is invalid, terminate the program.
    if program_terminator:
        quit()

    # Perform K-means clustering and print the final centroids.
    centroids = find_centroids(k, max_iterations, input_data)
    print_centroids(centroids)


# Run the main function if this script is run as the main module.
if __name__ == "__main__":
    main()
