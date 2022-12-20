import unittest
import itertools

import napf
import numpy as np


class InitAndQueryTest(unittest.TestCase):

    def test_init_and_query(self):
        dims = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        data_type = ["float64", "float32", "int64", "int32"]
        metric = [1, 2]


        for d, dt, m in itertools.product(dims, data_type, metric):
            # try to initialize the tree with
            n_data = 100
            randata = (np.random.random((n_data, d)) * n_data).astype(dt)
            tree_data = np.vstack(
                (randata, np.array([[-1] * d], dtype=dt))
            ).astype(dt) # make sure one more time 

            kdt = napf.KDT(tree_data, m)

            # init test
            qname = type(kdt.core_tree).__qualname__
            assert kdt.core_tree.dim == d,\
                f"wrong dim init for {qname}"
            assert kdt.core_tree.tree_data.dtype == dt,\
                f"wrong dtype init for {qname}"
            assert kdt.core_tree.metric == m,\
                f"wrong metric init for {qname}"

            # query test should be all be dist = 0 and id = n_data
            dist, ids = kdt.query([[-1] * d], nthread=1)
            assert np.isclose(dist[0], 0),\
                f"wrong dist query for {qname}"
            assert ids[0] == n_data,\
                f"wrong index query for {qname}"


if __name__ == "__main__":
    unittest.main()
