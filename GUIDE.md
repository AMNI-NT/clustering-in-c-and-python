# Contents and usage

Use Python 3.10+, NumPy, a C compiler and Python development headers. For silhouette analysis, install scikit-learn as well.

```sh
python -m pip install -r requirements.txt
python tests/run_checks.py
```

The tests compile the standalone C K-means executable and compare its centroids to the Python program on a small deterministic dataset. No downloads are needed by the tests.

## K-means

```sh
gcc kmeans/kmeans.c -lm -o /tmp/kmeans
python kmeans/kmeans.py 2 < examples/points.txt
/tmp/kmeans 2 < examples/points.txt
```

## Python/C extensions

Run each setup command inside its own directory:

```sh
cd symnmf
python setup.py build_ext --inplace
python symnmf.py 2 norm ../examples/points.txt
python symnmf.py 2 symnmf ../examples/points.txt
```

Likewise build `kmeans-plus-plus/setup.py` before running `kmeans_pp.py K [max_iter] epsilon file1.csv file2.csv`. The two CSV inputs carry keys in their first column. The original initialization uses distance weights rather than squared-distance weights. It isn't a standard K-means++ implementation. Degenerate/all-identical inputs can also lead to zero probability denominators.
