# napf
[`nanoflann`](https://github.com/jlblancoc/nanoflann) wrappers for python and maybe fortran.

## python
As `nanoflann` offers template classes, separate classes are implemented in `napf` for each _{datatype, data dimension, distance metric}_. All the search functions are equipped with multi-thread execution. Uses [`numpy.ndarray`](https://numpy.org/doc/stable/reference/generated/numpy.ndarray.html) for data input and output.  
Currently, the combinations of following options are supported:
- `data type`: {__double__, __float__, __int__, __long__}  _(corresponds to {np.float64, np.float32, np.int32, np.int64})_
- `data dimension`: {__1__, __2__, __3__, __4__, __5__, __6__, __7__, __8__, __9__, __10__}
- `distance metric`: {__L1__, __L2__}

## fortran
maybe...
