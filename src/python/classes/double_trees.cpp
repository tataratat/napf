#include "../pykdt.hpp"

void init_double_trees(py::module_& m) {
  add_kdt_pyclass<double, 1, 1>(m, "KDTdD1L1");
  add_kdt_pyclass<double, 1, 2>(m, "KDTdD1L2");
  add_kdt_pyclass<double, 2, 1>(m, "KDTdD2L1");
  add_kdt_pyclass<double, 2, 2>(m, "KDTdD2L2");
  add_kdt_pyclass<double, 3, 1>(m, "KDTdD3L1");
  add_kdt_pyclass<double, 3, 2>(m, "KDTdD3L2");
  add_kdt_pyclass<double, 4, 1>(m, "KDTdD4L1");
  add_kdt_pyclass<double, 4, 2>(m, "KDTdD4L2");
  add_kdt_pyclass<double, 5, 1>(m, "KDTdD5L1");
  add_kdt_pyclass<double, 5, 2>(m, "KDTdD5L2");
  add_kdt_pyclass<double, 6, 1>(m, "KDTdD6L1");
  add_kdt_pyclass<double, 6, 2>(m, "KDTdD6L2");
  add_kdt_pyclass<double, 7, 1>(m, "KDTdD7L1");
  add_kdt_pyclass<double, 7, 2>(m, "KDTdD7L2");
  add_kdt_pyclass<double, 8, 1>(m, "KDTdD8L1");
  add_kdt_pyclass<double, 8, 2>(m, "KDTdD8L2");
  add_kdt_pyclass<double, 9, 1>(m, "KDTdD9L1");
  add_kdt_pyclass<double, 9, 2>(m, "KDTdD9L2");
  add_kdt_pyclass<double, 10, 1>(m, "KDTdD10L1");
  add_kdt_pyclass<double, 10, 2>(m, "KDTdD10L2");
}
