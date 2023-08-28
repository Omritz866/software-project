import numpy as np
import sys
import symnmf  # Importing the C extension

def read_input(file_name):
    return np.loadtxt(file_name, delimiter=',')

def calculate_similarity(X):
    # your logic here
    pass

def calculate_ddg(X):
    # your logic here
    pass

def calculate_norm(X):
    # your logic here
    pass

if __name__ == "__main__":
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]
    X = read_input(file_name)

    if goal == "symnmf":
        # Initialize H, call symnmf from the C extension
        pass
    elif goal == "sym":
        print(calculate_similarity(X))
    elif goal == "ddg":
        print(calculate_ddg(X))
    elif goal == "norm":
        print(calculate_norm(X))
