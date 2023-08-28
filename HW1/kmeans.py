import math
import sys
MAX_ITER = 1000
DEFAULT_ITER = 200
EPSILON = 0.001


def buildkMeans():
    file_name = sys.argv[len(sys.argv)-1]
    with open(file_name, 'r') as file:
        lines = file.readlines()

    # Initialize the 2D array
    kMeans = []

    # Iterate over the lines and convert each line to a list of floats
    for line in lines:
        # Split the line by commas and convert each element to float
        row = [float(num) for num in line.strip().split(',')]
        kMeans.append(row)

    return kMeans



#splits the input file into a list of vectors 
def inputToVectorsList(inputData):
    s = open(inputData,'r')
    vectorsList = s.readlines()
    for i in range(len(vectorsList)):
        vectorsList[i] = [float(j) for j in vectorsList[i].split(',')]
    return (vectorsList)



#printing in the requested foramt
def finishPrint(vectorsList):
    for vector in vectorsList:
        rounded_vector = [round(x, 4) for x in vector]
        print(','.join(str(x) for x in rounded_vector))
    print("")

#Calculates distance between two vectors 
def distance(a, b):
    distance = 0
    for i in range(len(a)):
        distance = distance +  ((a[i]-b[i])**2) 
    return math.sqrt(distance)


#Calculates new centroid for requested cluster
def centroidCalculator(cluster):
    newCentroid = [0 for x in cluster[0]]
    for vector in cluster:
        for i in range(len(vector)):
            newCentroid[i] = newCentroid[i] + vector[i]
    
    for i in range(len(newCentroid)):
        newCentroid[i] = newCentroid[i]/len(cluster)
    return newCentroid



#Updates all centroids
def updateCentroids(clustersList, centroidsList,K):
    
    epsilon = 0.001
    epsilonIndicator = True
    for i in range(K):
        newCentroid = centroidCalculator(clustersList[i])
        if (distance(newCentroid,centroidsList[i]) >= epsilon):
            epsilonIndicator = False
        centroidsList[i] = newCentroid
    return epsilonIndicator
    
#running the main,prints relevant error message and exits if neccesery
if __name__ == "__main__":
    vectorsList = buildkMeans()
    
    #K eshkolot   
    K = int(sys.argv[1])
    # Number of rows
    N = len(vectorsList)
    
    # Number of columns (assuming all rows have the same number of elements)
    D = len(vectorsList[0])
    # checking variables conditions
    
    if len(sys.argv) == 4:  # in cases of iter input
        iter = int(sys.argv[2])
        if iter < 2 or iter > MAX_ITER - 1:  # checks if iter valid
            print("Invalid maximum iteration!")
            sys.exit(1)
    elif len(sys.argv) == 3:  # case of default iter input
        iter = DEFAULT_ITER
    else:  # otherwise will fail
        print("An Error Has Occurred")
        sys.exit(1)

    if K < 2 or K > N-1 :  # checks if K valid
        print("Invalid number of clusters!")
        sys.exit(1)



    centroidsList = [i for i in vectorsList[:K]]#array of centroids with k length
    clustersList = [[centroid] for centroid in centroidsList] # k reshimot
    iterCounter = 0
    epsilonIndicator = False
    while not epsilonIndicator and iterCounter<iter:
        for i in range(N): #run over vector list
            currVector = vectorsList[i]
            minDist = float('inf')
            minIndex = -1
            for j in range(K): #run over centroids list
                dist = distance(currVector, centroidsList[j])
                if(dist < minDist):#finding relevant cluster
                    minDist = dist
                    minIndex = j
            for h in range (K):
              if clustersList[h].count(currVector) >= 1:
              #  if currVector in clustersList[h]:
                
                   clustersList[h].remove(currVector)
            for h in range (K):
                if minIndex == h:        
                    clustersList[h].append(currVector)#updating relevant cluster
        
        iterCounter += 1    

        epsilonIndicator = updateCentroids(clustersList, centroidsList,K)#check if all the cluster has changed according to the terms 
    finishPrint(centroidsList)