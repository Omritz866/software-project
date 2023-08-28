import math
import sys

def k_means(K, iter, input_data):
    vectors_list, vector_len = input_to_vectors(input_data)
    N = len(vectors_list)

    invalidK = False
    invalidIter = False
    if (K <= 1 or K>= N):
        invalidK = True
    if (iter <= 1 or iter >= 1000):
        invalidIter = True
    errorMessage(invalidK, invalidIter)
        
    centroids_list = [x for x in vectors_list[:K]]
    clusters_list = [[centroid] for centroid in centroids_list]
    delta_flag = False
    iteration_number = 0
    while iteration_number < iter and not delta_flag:
        for i in range(N): #traverse over vector list
            x = vectors_list[i]
            min_dist = math.inf
            min_index = 0
            for j in range(K): #traverse over centroids list
                dist = euc_distance(x, centroids_list[j])
                if(dist < min_dist):
                    min_dist = dist
                    min_index = j
            for j in range(K):
                if x in clusters_list[j]:
                    clusters_list[j].remove(x)
                if j == min_index:
                    clusters_list[j].append(x)
        delta_flag = update_centroids(clusters_list, centroids_list,K)
        iteration_number += 1
    print_formatted(centroids_list)

"""
prints the output in the requested format
"""
def print_formatted(vectors_list):
    for vector in vectors_list:
        for i in range(len(vector)):
            vector[i] = "%.4f" % vector[i]
        print(','.join(str(x) for x in vector))
    print("")
"""
Calculates the new centroid of a cluster
"""
def calc_centroid(cluster):
    new_centroid = [0 for coor in cluster[0]]
    for vector in cluster:
        for i in range(len(vector)):
            new_centroid[i] += vector[i]
    
    for i in range(len(new_centroid)):
        new_centroid[i] /= len(cluster)
    return new_centroid


"""
Updates all centroids
"""
def update_centroids(clusters_list, centroids_list,K):
    
    delta_flag = True
    for i in range(K):
        new_centroid = calc_centroid(clusters_list[i])
        if (euc_distance(new_centroid,centroids_list[i]) >= 0.001):
            delta_flag = False
        centroids_list[i] = new_centroid
    return delta_flag
    
"""
Calculates the euclidean distance between two vectors (of the same length)
"""
def euc_distance(x_vector, y_vector):
    sum = 0
    for i in range(len(x_vector)):
        sum += ((x_vector[i] - y_vector[i])**2) 
    return math.sqrt(sum)

"""
This function splits the input (.txt) into a list of vectors (represnted as lists also)
"""
def input_to_vectors(input_data):
    f = open(input_data,'r')
    vectors_list = f.readlines()
    for i in range(len(vectors_list)):
        vectors_list[i] = [float(x) for x in vectors_list[i].split(',')]
    vector_len = len(vectors_list[0])
    return (vectors_list, vector_len)

"""
Prints relevant error message then exits
"""
def errorMessage(invalidK, invalidIter):
    if invalidK:
        print("Invalid number of clusters!")
    if invalidIter:
        print("Invalid maximum iteration!")
    if (invalidIter or invalidK):
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) == 4:
        k_means(int(sys.argv[1]), int(sys.argv[2]), sys.argv[3])
    elif len(sys.argv) == 3:
        k_means(int(sys.argv[1]), 200, sys.argv[2])
    else:
        print("An Error Has Occurred")
        sys.exit(1)


