#include "../pykdt.hpp"

void init_float_trees(py::module_& m) {
  add_kdt_pyclass<float, 1, 1>(m, "KDTfD1L1");
  add_kdt_pyclass<float, 1, 2>(m, "KDTfD1L2");
  add_kdt_pyclass<float, 2, 1>(m, "KDTfD2L1");
  add_kdt_pyclass<float, 2, 2>(m, "KDTfD2L2");
  add_kdt_pyclass<float, 3, 1>(m, "KDTfD3L1");
  add_kdt_pyclass<float, 3, 2>(m, "KDTfD3L2");
  add_kdt_pyclass<float, 4, 1>(m, "KDTfD4L1");
  add_kdt_pyclass<float, 4, 2>(m, "KDTfD4L2");
  add_kdt_pyclass<float, 5, 1>(m, "KDTfD5L1");
  add_kdt_pyclass<float, 5, 2>(m, "KDTfD5L2");
  add_kdt_pyclass<float, 6, 1>(m, "KDTfD6L1");
  add_kdt_pyclass<float, 6, 2>(m, "KDTfD6L2");
  add_kdt_pyclass<float, 7, 1>(m, "KDTfD7L1");
  add_kdt_pyclass<float, 7, 2>(m, "KDTfD7L2");
  add_kdt_pyclass<float, 8, 1>(m, "KDTfD8L1");
  add_kdt_pyclass<float, 8, 2>(m, "KDTfD8L2");
  add_kdt_pyclass<float, 9, 1>(m, "KDTfD9L1");
  add_kdt_pyclass<float, 9, 2>(m, "KDTfD9L2");
  add_kdt_pyclass<float, 10, 1>(m, "KDTfD10L1");
  add_kdt_pyclass<float, 10, 2>(m, "KDTfD10L2");
}
