import math
import sys
import numpy as np
import pandas as pd
import kmeanssp

MAX_ITER = 1000
DEFAULT_ITER = 300


#---------Merging two text files---------
def mergeFiles(file_name_1, file_name_2):
    df1 = pd.read_csv(file_name_1, sep=',', header=-1)  # read the first text file into a dataframe
    df2 = pd.read_csv(file_name_2, sep=',', header=-1)  # read the second text file into a dataframe
    result = pd.merge(df1, df2, how='inner', left_on=0, right_on=0)  # perform an inner join on the two dataframes using the index as the key
    result.to_csv('result.txt', sep=',')  # write the result to a new text file
    return result.values.tolist(), len(result.columns), result.shape[0]  # return the merged file, D, and N


#-------calculate distance between two vectors-------
def distance(a, b):
    distance = 0
    for i in range(len(a)):
        distance = distance + ((a[i] - b[i]) ** 2)
    return math.sqrt(distance)


#-----------calculate the sum of all the D(x) for all of the data points------
def calculateTotalDistance(distances):
    total = 0
    for i in range(len(distances)):
        total = total + distances[i]
    return total


#----------build probabilities array for all the data points-----
def buildPro(distances, total):
    pro = [0 for _ in range(len(distances))]  # init with zeros
    for i in range(len(distances)):
        pro[i] = distances[i] / total
    return pro


#----printing probabilities-------
def proPrint(pro):
    x = 0
    for element in pro:
        print(f"probability of data point {x} = {element}\n")
        x = x + 1


#-------printing Centers index-------
def indexesPrint(centerIndexes):
    index_str = ','.join(str(x) for x in centerIndexes[:-1])
    index_str += ',' + str(centerIndexes[-1])
    print(index_str)


#----printing Centers--------
def centerPrint(centers):
    for i in range(len(centers)):
        print(','.join(str(x) for x in centers[i]))


#-----updating new array of centers suits for C code-----
def updateCenters(centerList):
    newCenters = []
    for i in range(len(centerList)):
        new = tuple(centerList[i])
        newCenters.append(tuple(new))
    return newCenters


if __name__ == "__main__":
    # ---------Reading arguments--------
    K = int(sys.argv[1])  # Number of required clusters.

    if len(sys.argv) == 6:  # in cases of iter input
        iter = int(sys.argv[2])
        if iter < 2 or iter > MAX_ITER - 1:  # checks if iter valid
            print("Invalid maximum iteration!")
            sys.exit(1)
        eps = sys.argv[3]
        file_name_1 = sys.argv[4]
        file_name_2 = sys.argv[5]
        currFile, D, N = mergeFiles(file_name_1, file_name_2)
    elif len(sys.argv) == 5:  # case of default iter input
        iter = DEFAULT_ITER
        eps = sys.argv[2]
        file_name_1 = sys.argv[3]
        file_name_2 = sys.argv[4]
        currFile, D, N = mergeFiles(file_name_1, file_name_2)
    else:  # otherwise will fail
        print("An Error Has Occurred")
        sys.exit(1)
    currFile = sorted(currFile, key=lambda x: x[0])  # sorting the array according to the first column values
    currFile = [row[1:] for row in currFile]  # Remove the first column from each row
    currFile = np.array(currFile)
    totalDistance = 0
    np.random.seed(0)
    centerList = []
    centerIndexes = []
    z = 0
    if N > 0:
        z = np.random.choice(N)
    else:
        print("N must be greater than zero.")
        sys.exit(1)

    centerList.append(currFile[z])  # add to centerList the z row of currFile
    centerIndexes.append(z)

    while len(centerList) < K:  # until we build K centers
        distances = [0 for _ in range(N)]  # array of P(d) of each data point of zeros
        for i in range(N):  # running on N data points
            minDist = float('inf')
            localPoint = tuple(currFile[i])  # turn the row of i to be able to compare to regular vector
            if not localPoint in [tuple(x) for x in centerList]:  # checks that the data point has not been chosen yet as a center
                for j in range(len(centerList)):  # calculating the shortest distance into distance [i] for the i vector
                    dist = distance(localPoint, centerList[j])
                    if dist < minDist:
                        minDist = dist
                distances[i] = minDist

        totalDistance = calculateTotalDistance(distances)  # totalDistance = sum of all data points distances from the closest center fo them
        pro = buildPro(distances, totalDistance)  # building an array of probabilities for each data point
        t = np.random.choice(N, replace=False, p=pro)  # choose random number while considering probabilities
        centerList.append(currFile[t])  # append new center to the center list
        centerIndexes.append(t)  # adding the new center index to the center indexes list

    Ucenters = updateCenters(centerList)
    centerList = np.array(centerList)
    #print(currFile)
    #print(centerList)

    # -------printing---------
    # proPrint(pro)
    # indexesPrint(centerIndexes)
    # centerPrint(Ucenters)
    #python3 setup.py build_ext --inplace
    #python3 kmeans_pp.py 3 100 0.01 input_1_db_1.txt input_1_db_2.txt

    
    
    #print(currFile)
    eps = float(eps)
    try:
        centroids = kmeanssp.fit(centerList,currFile,K,D,N,iter,eps)
    except RuntimeError as e:
        print(str(e))
        quit()