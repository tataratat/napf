import numpy as np

from napf import _napf as core

np2napf_dtypes = {
    "int32" : "i",
    "int64" : "l",
    "float32" : "f",
    "float64" : "d",
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

    except:
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
    core_class_str
    """
    arr = np.asanyarray(tree_data)
    dtypestr = str(arr.dtype)
    if dtypestr not in np2napf_dtypes:
        raise TypeError(
            f"Sorry, `napf` does not support ({dtypestr}) dtypes."
        )

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


def KDT(tree_data, metric=1):
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
      Default is 1. Valid options are {1, l1, L1, 2, l2, L2}.

    Returns
    --------
    core_obj: KDT{data_t}D{dim}L{metric}
    """
    tdata = np.ascontiguousarray(tree_data)
    corecls = core_class_str(tdata, metric)

    return eval(f"core.{corecls}(tdata)")
