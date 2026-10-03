import kmeanssp as kmeans
import numpy as np
import sys

def kmeans_pp_initialization(data, K):
    np.random.seed(1234)
    centroids = []
    data = np.array(data)
    chosen = set()

    # Choose one center uniformly 
    random_index = np.random.choice(len(data))
    centroids.append(data[random_index].tolist())
    chosen.add(random_index)

    # Main loop
    while len(centroids) < K:

        distances = {}
        for i, data_point in enumerate(data):
            if i in chosen: 
                distances[i] = 0
            else: 
                distances[i] = np.min(np.linalg.norm(data_point - np.array(centroids), axis=1))

        sum_of_distances = sum(distances.values())
        probabilities =  {key: distances[key] / sum_of_distances for key in distances}

        random_index = np.random.choice(list(probabilities.keys()), p=list(probabilities.values()))
        centroids.append(data[random_index].tolist())
        chosen.add(random_index)
       

    return centroids, chosen

# def join_data(data1, data2):
    
 
# iter was provided
if len(sys.argv) == 6:
    K = int(sys.argv[1])
    max_iter = int(sys.argv[2])
    eps = float(sys.argv[3])
    file_name_1 = sys.argv[4]
    file_name_2 = sys.argv[5]

# iter was not provided
elif len(sys.argv) == 5:
    K = int(sys.argv[1])
    max_iter = 300
    eps = float(sys.argv[2])
    file_name_1 = sys.argv[3]
    file_name_2 = sys.argv[4]

data1 = np.genfromtxt(file_name_1, delimiter=',')
data2 = np.genfromtxt(file_name_2, delimiter=',')


# input validation
if K <= 1 or K>=len(data1 ):
    print("Incorrect number of clusters!")
    sys.exit(1)

if max_iter <= 1 or max_iter >= 800:
    print("Incorrect maximum iteration!")
    sys.exit(1)

if eps < 0:
    print("Incorrect epsilon!")
    sys.exit(1)


keys = np.intersect1d(data1[:, 0], data2[:, 0])
joined_data = []

for key in keys:
    row1 = data1[data1[:, 0] == key][0]
    row2 = data2[data2[:, 0] == key][0]
    joined_data.append(np.concatenate((row1, row2[1:])))

# convert joined data to python array
joined_data = np.vstack(joined_data)

# sort by keys in ascending order
joined_data = joined_data[joined_data[:, 0].argsort()]

# remove keys from data
joined_data_no_keys = joined_data[:, 1:]

joined_data = joined_data.tolist()
joined_data_no_keys = joined_data_no_keys.tolist()


initial_centroids, chosen = kmeans_pp_initialization(joined_data_no_keys, K)


centroids = kmeans.fit(initial_centroids, joined_data_no_keys, K, max_iter, eps)

#output chosen centroids indices
print(','.join(f"{value:.0f}" for value in chosen))

# Output centroids
for row in centroids:
    print(','.join(f"{value:.4f}" for value in row))

sys.exit(0)