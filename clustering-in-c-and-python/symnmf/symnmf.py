from fileinput import filename
import sys
import numpy as np
import symnmf_module
 
def print_matrix(matrix):
    """Print a matrix with values formatted to 4 decimal places"""
    for row in matrix:
        print(','.join(map(lambda x: f"{x:.4f}", row)))

def error():
    """An Error Has Occurred"""
    print("An Error Has Occurred")
    sys.exit()

def sym(data):
    """Calculate the symmetric similarity matrix"""
    return symnmf_module.sym_p(data)

def ddg(data):
    """Calculate the diagonal degree matrix"""
    return symnmf_module.ddg_p(data)

def norm(data):
    """Calculate the normalized similarity matrix"""
    return symnmf_module.norm_p(data)

def symnmf(data, k):
    """Perform SymNMF algorithm and return the resulting matrix H"""
    # Calculate the normalized similairy matrix
    W = symnmf_module.norm_p(data)
    
    # Randomly initialize H with values from the interval [0, 2 ∗ sqrt(m/k)], where m is the average of all entries of W.
    H = np.random.uniform(0, 2 * np.sqrt(np.mean(W) / k), size=(len(W), k)).tolist()
    
    return symnmf_module.symnmf_p(H, W)

if __name__ == "__main__":
    np.random.seed(1234)

    # validate arg count
    if len(sys.argv) != 4:
        error()

    try:
        k = int(sys.argv[1])
        goal = sys.argv[2]
        filename = str(sys.argv[3])
    except:
        error()

    # validate filename
    if not filename.endswith('.txt'):
        error()

    # validate file's existance
    try:
        with open(filename, 'r') as file:
            pass
    except:
        error()

    # read data file
    X = np.genfromtxt(filename, delimiter=',')
    if X.ndim == 1:
        X = X.reshape(-1, 1)
    X = X.tolist()

    # validate k 
    if k <= 0 or k >= len(X):
        error()

    # Call method corresponding with goal
    match goal: 
        case "sym":
            return_matrix = sym(X)
        
        case "ddg":
            return_matrix = ddg(X)

        case "norm":
            return_matrix = norm(X)

        case "symnmf":
            return_matrix = symnmf(X, k)
        case _: # invalid goal
            error()

    # Print the resulting matrix
    print_matrix(return_matrix)
