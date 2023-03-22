import numpy as np

from napf import _napf as core  # noqa: F401

np2napf_dtypes = {
    "int32": "i",
    "int64": "l",
    "float32": "f",
    "float64": "d",
}


def validate_metric_input(metric):
    """
    internal use fn for metric validation

    Parameters
    -----------
    metric: int or str

    Returns
    --------
    m: int
      validated metric casted to int.
    """
    try:
        m = int(metric)
        assert m in [1, 2]

    except ValueError:
        mstr = str(metric).lower()
        print(mstr)
        assert mstr.startswith("l1") or mstr.startswith("l2")
        m = int(mstr[-1])
        assert m in [1, 2]

    except BaseException:
        raise ValueError(
            "KDT only supports 1 or 2 (alternatively 'L1' or 'L2') "
            "as metric input."
        )

    return m


def core_class_str(tree_data, metric):
    """
    Returns class name of current setting.
    Also checks if it is valid dtype.

    Parameters
    -----------
    tree_data: (n, dim) np.ndarray
    metric: int or str

    Returns
    --------
    core_class_str: str
    """
    arr = np.asanyarray(tree_data, dtype=getattr(tree_data, "dtype", None))
    dtypestr = str(arr.dtype)
    if dtypestr not in np2napf_dtypes:
        raise TypeError(f"Sorry, `napf` does not support ({dtypestr}) dtypes.")

    if arr.ndim != 2:
        raise ValueError(
            "Please make sure your input array is 2D! "
            "Given array's dim is {arr.ndim}."
        )

    # extract info
    data_t = np2napf_dtypes[dtypestr]
    dim = arr.shape[1]
    metric = validate_metric_input(metric)

    return f"KDT{data_t}D{dim}L{metric}"


class _KDT:
    """
    Helper class to set default nthread and add documentations.
    """

    __slots__ = (
        "_core_tree",
        "_nthread",
    )

    def __init__(self, tree_data, metric=2, nthread=1):
        """
        _KDT init. Given tree_data, creates corresponding core kdt class.
        Tree is initialized using `newtree()`.

        Attributes
        -----------
        core_tree: napf.core.KDT*
        tree_data: (n, d) np.ndarray
          {double, float, int, long}
          Readonly tree data. saved with `newtree()`
        nthread: int
          Default value for nthreads.

        Parameters
        -----------
        tree_data: (n, d) np.ndarray
          {double, float, int, long}
        """
        self.newtree(tree_data, metric)
        self.nthread = nthread

    @property
    def nthread(self):
        """
        Returns saved default value for nthread.

        Parameters
        -----------
        None

        Returns
        --------
        nthread: int
        """
        return self._nthread

    @nthread.setter
    def nthread(self, nthread_):
        """
        Sets default value for nthread.

        Parameters
        -----------
        nthread_: int

        Returns
        --------
        None
        """
        self._nthread = nthread_

    @property
    def core_tree(self):
        """
        Returns initialized core tree, if there's any.

        Parameters
        -----------
        None

        Returns
        --------
        core_tree: napf.core.KDT*
        """
        if hasattr(self, "_core_tree"):
            return self._core_tree

        else:
            return None

    @property
    def tree_data(self):
        """
        Returns data used to initialize core tree. Read only

        Parameters
        -----------
        None

        Returns
        --------
        tree_data: (n, d) np.ndarray
          {double, float, int, long}
        """
        coretree = self.core_tree
        if coretree is not None:
            return coretree.tree_data

        else:
            return None

    def newtree(self, tree_data, metric=1):
        """
        Given 2D array-like tree_data, it:
          1. makes sure data is a contiguous array
          2. builds a corresponding kdt.
        Note that core kdt objects also have `newtree()` function with the same
        parameters, and build a new kdt each time it is called.

        Parameters
        -----------
        tree_data: (n, d) np.ndarray
          {double, float, int, long}
        """
        tdata = np.ascontiguousarray(tree_data)
        core_cls = core_class_str(tdata, metric)  # checks and raises error
        # we can call newtree() function of the core class,
        # if _core_tree already exists.
        # However, creating a new kdt should not add significant overhead.
        self._core_tree = eval(f"core.{core_cls}(tdata)")

    def knn_search(self, queries, kneighbors, nthread=None):
        """
        k-nearest-neighbor search.

        Parameters
        -----------
        queires: (m, d) np.ndarray
          Data type will be casted to the same type as `tree_data`.
        kneighbors: int
        nthread: int
          Default is None and will use self.nthread.

        Returns
        --------
        ids_and_distances: tuple
          ((m, kneighbors) np.ndarray - double dists,)
           (m, kneighbors) np.ndarray - uint ids)
        """
        if nthread is None:
            nthread = self.nthread

        return self.core_tree.knn_search(queries, kneighbors, nthread)

    def query(self, queries, nthread=None):
        """
        scipy-like KDTree query call.
        Same as `knn_search(queries, 1, nthreads)`

        Parameters
        -----------
        queries: (m, d) np.ndarray
          Data type will be casted to the same type as `tree_data`.
        nthread: int
          Default is None and will use self.nthread.

        Returns
        --------
        ids_and_distances: tuple
          ((m, 1) np.ndarray - double dists,)
           (m, 1) np.ndarray - uint ids)
        """
        if nthread is None:
            nthread = self.nthread

        return self.core_tree.query(queries, nthread)

    def radius_search(self, queries, radius, return_sorted, nthread=None):
        """
        Searches for neighbors in given radius.

        Parameters
        -----------
        queries: (m, d) np.ndarray
          Data type will be casted to the same type as `tree_data`.
        radius: float
        return_sorted: bool
        nthread: int
          Default is None and will use self.nthread

        Returns
        --------
        ids_and_distances: tuple
          ((m, 1) np.ndarray - uint ids,
           (m, 1) np.ndarray - double dists)
        """
        if nthread is None:
            nthread = self.nthread

        return self.core_tree.radius_search(
            queries,
            radius,
            return_sorted,
            nthread,
        )

    def radii_search(self, queries, radii, return_sorted, nthread=None):
        """
        Similar to `radius_search`, but you can specify radius for each query.

        Parameters
        -----------
        queries: (m, d) np.ndarray
          Data type will be casted to the same type as `tree_data`.
        radii: (m,) np.ndarray
        return_sorted: bool
        nthread: int
          Default is None and will use self.nthread

        Returns
        --------
        ids_and_distances: tuple
          ((m, 1) np.ndarray - uint ids,
           (m, 1) np.ndarray - double dists)
        """
        # input size check
        if len(queries) != len(radii):
            raise ValueError(
                f"Input size mismatch between queires ({len(queries)}) "
                f" and radii ({len(radii)})."
                "They should be the same."
            )

        if nthread is None:
            nthread = self.nthread

        return self.core_tree.radii_search(
            queries,
            radii,
            return_sorted,
            nthread,
        )


def KDT(tree_data, metric=2):
    """
    Factory like initializer for KDT.
    `napf` is implemented as template, thus, there are separate classes
    for each {data_type, dim, metric}.
    Currently following combinations are supported:
    data_type: {double, int}
    dim: {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
    metric: {L1, L2}

    Parameters
    -----------
    tree_data: (n, dim) np.ndarray
      Default is None. double or int.
    metric: int or str
      Default is 2 and distance will be a squared euklidian distance.
      Valid options are {1, l1, L1, 2, l2, L2}.


    Returns
    --------
    core_obj: KDT{data_t}D{dim}L{metric}
    """
    tdata = np.ascontiguousarray(tree_data)

    return _KDT(tdata, metric)
