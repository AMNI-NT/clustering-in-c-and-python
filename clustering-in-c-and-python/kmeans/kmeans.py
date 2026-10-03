import sys 

lines = sys.stdin.read().strip().split('\n')

matrix = [list(map(float, line.split(','))) for line in lines]

# Read arguments
if len(sys.argv) >= 2:
    k = int(sys.argv[1])
else:
    sys.exit(1)

if len(sys.argv) >= 3:
    max_iter = int(sys.argv[2])
else:
    max_iter = 400

# Input validation
if k <= 1 or k>=len(matrix):
    print("Incorrect number of clusters!")
    sys.exit(1)

if max_iter <= 1 or max_iter >= 800:
    print("Incorrect maximum iteration!")
    sys.exit(1)

# Initialize centroids (first k points)
centroids = matrix[:k]

for iteration in range(max_iter):
    clusters = [[] for _ in range(k)]

    # Assign points to the nearest centroid
    for vector in matrix:
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

# Output centroids
for row in centroids:
    print(','.join(f"{value:.4f}" for value in row))