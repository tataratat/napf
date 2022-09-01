#include "../pykdt.hpp"

void init_long_trees(py::module_& m) {
  add_kdt_pyclass<long, 1, 1>(m, "KDTlD1L1");
  add_kdt_pyclass<long, 1, 2>(m, "KDTlD1L2");
  add_kdt_pyclass<long, 2, 1>(m, "KDTlD2L1");
  add_kdt_pyclass<long, 2, 2>(m, "KDTlD2L2");
  add_kdt_pyclass<long, 3, 1>(m, "KDTlD3L1");
  add_kdt_pyclass<long, 3, 2>(m, "KDTlD3L2");
  add_kdt_pyclass<long, 4, 1>(m, "KDTlD4L1");
  add_kdt_pyclass<long, 4, 2>(m, "KDTlD4L2");
  add_kdt_pyclass<long, 5, 1>(m, "KDTlD5L1");
  add_kdt_pyclass<long, 5, 2>(m, "KDTlD5L2");
  add_kdt_pyclass<long, 6, 1>(m, "KDTlD6L1");
  add_kdt_pyclass<long, 6, 2>(m, "KDTlD6L2");
  add_kdt_pyclass<long, 7, 1>(m, "KDTlD7L1");
  add_kdt_pyclass<long, 7, 2>(m, "KDTlD7L2");
  add_kdt_pyclass<long, 8, 1>(m, "KDTlD8L1");
  add_kdt_pyclass<long, 8, 2>(m, "KDTlD8L2");
  add_kdt_pyclass<long, 9, 1>(m, "KDTlD9L1");
  add_kdt_pyclass<long, 9, 2>(m, "KDTlD9L2");
  add_kdt_pyclass<long, 10, 1>(m, "KDTlD10L1");
  add_kdt_pyclass<long, 10, 2>(m, "KDTlD10L2");
}
