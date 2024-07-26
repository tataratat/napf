import itertools
import unittest

import numpy as np

import napf


def loop_all_and_test(dims, data_type, metrics, test_func):
    for dim, data_t, metric in itertools.product(dims, data_type, metrics):
        test_func(dim, data_t, metric)


class InitAndQueryTest(unittest.TestCase):
    def test_init_and_query(self):
        dims = list(range(1, 21))
        data_type = ["float64", "float32", "int64", "int32"]
        metrics = [1, 2]

        def test_func(dim, data_t, metric):
            # try to initialize the tree with
            n_data = 100
            randata = (np.random.random((n_data, dim)) * n_data).astype(data_t)
            tree_data = np.vstack(
                (randata, np.array([[-1] * dim], dtype=data_t))
            ).astype(
                data_t
            )  # make sure one more time

            kdt = napf.KDT(tree_data, metric)

            # init test
            qname = type(kdt.core_tree).__qualname__
            assert kdt.core_tree.dim == dim, f"wrong dim init for {qname}"
            assert (
                kdt.core_tree.tree_data.dtype == data_t
            ), f"wrong dtype init for {qname}"
            assert (
                kdt.core_tree.metric == metric
            ), f"wrong metric init for {qname}"

            # query test should be all be dist = 0 and id = n_data
            dist, ids = kdt.query([[-1] * dim], nthread=1)
            assert np.isclose(dist[0], 0), f"wrong dist query for {qname}"
            assert ids[0] == n_data, f"wrong index query for {qname}"

            # test knn_search
            dist, ids = kdt.knn_search(kdt.tree_data, 1, nthread=2)
            assert np.isclose(
                dist.sum(), 0
            ), f"wrong dist query for {qname}, dim {dim}"

            # skip integer types for index check
            # as it is too easy for them to have duplicates.
            # with default options of nanoflann, this will return smaller index
            if data_t.startswith("int"):
                return

            assert np.all(
                ids.ravel() == np.arange(len(kdt.tree_data))
            ), f"wrong index query for {qname}, dim {dim}"

        loop_all_and_test(dims, data_type, metrics, test_func)

    def test_rknn(self):
        dims = list(range(1, 21))
        data_type = ["float64", "float32"]
        metrics = [1, 2]

        def test_func(dim, data_t, metric):
            n_data = 100
            random_data = (np.random.random((n_data, dim)) * n_data).astype(
                data_t
            )

            # create exactly two matches
            tree_data = np.vstack((random_data, random_data)).astype(data_t)

            kdt = napf.KDT(tree_data, metric)

            nn = 1
            ids, dists = kdt.rknn_search(random_data, 1e-10, nn)
            assert ids.shape[1] == nn
            assert dists.shape[1] == nn
            # we can't guarantee ids but distance should be all zero
            assert np.isclose(dists.sum(), 0)

            # with two queries, also zero dist
            nn = 2
            ids, dists = kdt.rknn_search(random_data, 1e-10, nn)
            assert dists.shape[1] == nn
            assert ids.shape[1] == nn
            assert np.isclose(dists.sum(), 0)

            # with five, only first two queries are zero and last three are
            # dummy values
            nn = 5
            ids, dists = kdt.rknn_search(random_data, 1e-10, 5)
            assert dists.shape[1] == nn
            assert ids.shape[1] == nn
            assert np.isclose(dists[:, :2].sum(), 0)
            # dummy dist is negative for floats
            assert (dists[:, 2:] < 0).all()
            # dummy id must be bigger than n_data, as long as n_data does
            # not overflow
            assert (abs(ids[:, 2:]) > n_data * 2).all()

        loop_all_and_test(dims, data_type, metrics, test_func)


if __name__ == "__main__":
    unittest.main()
