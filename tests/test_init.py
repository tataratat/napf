import unittest
import itertools

import napf
import numpy as np


class InitTest(unittest.TestCase):

    def test_init(self):
        dims = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        data_type = ["float64", "float32", "int64", "int32"]
        metric = [1, 2]


        for d, dt, m in itertools.product(dims, data_type, metric):
            # try to initialize the tree with 
            tree_data = np.arange(10 * d, dtype=dt).reshape(10, d)
            kdt = napf.KDT(tree_data, m)

            qname = type(kdt.core_tree).__qualname__
            assert kdt.core_tree.dim == d,\
                f"wrong dim init for {qname}"
            assert kdt.core_tree.tree_data.dtype == dt,\
                f"wrong dtype  init for {qname}"
            assert kdt.core_tree.metric == m,\
                f"wrong metric init for {qname}"


if __name__ == "__main__":
    unittest.main()
