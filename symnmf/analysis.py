import sys
import symnmf
import symnmf_module
import numpy as np
import sklearn.metrics as metrics

MAX_ITER = 300

def error():
    """An Error Has Occurred"""
    print("An Error Has Occurred")
    sys.exit()

def calculate_symnmf_clusters(X, k):
    """Calculate cluster assignments using SymNMF"""
    H = np.array(symnmf.symnmf(X, k))
    return H.argmax(axis=1)

def calculate_kmeans_clusters(X, k):
    """Calculate cluster assignments using K-Means """
    # Initialize centroids (first k points)
    centroids = X[:k]

    for iteration in range(MAX_ITER):
        clusters = [[] for _ in range(k)]

        # Assign points to the nearest centroid
        for vector in X:
            distances = [sum((v - c) ** 2 for v, c in zip(vector, centroid)) for centroid in centroids]
            closest_cluster = distances.index(min(distances))
            clusters[closest_cluster].append(vector)

        # Update centroids
        new_centroids = []
        for cluster in clusters:
            if cluster:
                new_centroids.append([sum(col) / len(cluster) for col in zip(*cluster)])

            else:
                new_centroids.append(centroids[len(new_centroids)])
        centroids = new_centroids

    # end of kmeans
    # return cluster assignments
    return [clusters.index(cluster) for vector in X for cluster in clusters if vector in cluster]

np.random.seed(1234)

# validate arg count
if len(sys.argv) != 3:
    error()

k = int(sys.argv[1])

filename = str(sys.argv[2])

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

# Calculate cluster assignments for both methods
symnmf_cluster = calculate_symnmf_clusters(X, k)
kmeans_cluster = calculate_kmeans_clusters(X, k)

# Calculate and print silhouette scores for both methods
print("nmf: %.4f" % metrics.silhouette_score(X, symnmf_cluster))
print("kmeans: %.4f" % metrics.silhouette_score(X, kmeans_cluster))





