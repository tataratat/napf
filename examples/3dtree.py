import numpy as np

import napf

if __name__ == "__main__":
    tree_3d_data = np.zeros((15, 3))
    tree_3d_data[:, 0] = np.arange(15) / 10

    kdt3d = napf.KDT(tree_3d_data, metric=1)
    print("/****************************************************************/")
    print("tree data:")
    print(tree_3d_data)
    print("/****************************************************************/")

    queries = tree_3d_data[:7] + [0.04, 0, 0]

    print("/****************************************************************/")
    print("queries:")
    print(queries)
    print("/****************************************************************/")

    print("/****************************************************************/")
    print("knn_search, 2 neighbors:")
    print(kdt3d.knn_search(queries, 2))
    print("/****************************************************************/")

    print("/****************************************************************/")
    print("radius_search, radius = .061, return_sorted=True:")
    ids, dist = kdt3d.radius_search(queries, 0.061, True)
    print("  ids:")
    print(ids)
    print(" dist:")
    print(dist)
    print("/****************************************************************/")
