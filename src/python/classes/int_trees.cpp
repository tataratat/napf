#include "../pykdt.hpp"

void init_int_trees(py::module_& m) {
  add_kdt_pyclass<int, 1, 1>(m, "KDTiD1L1");
  add_kdt_pyclass<int, 1, 2>(m, "KDTiD1L2");
  add_kdt_pyclass<int, 2, 1>(m, "KDTiD2L1");
  add_kdt_pyclass<int, 2, 2>(m, "KDTiD2L2");
  add_kdt_pyclass<int, 3, 1>(m, "KDTiD3L1");
  add_kdt_pyclass<int, 3, 2>(m, "KDTiD3L2");
  add_kdt_pyclass<int, 4, 1>(m, "KDTiD4L1");
  add_kdt_pyclass<int, 4, 2>(m, "KDTiD4L2");
  add_kdt_pyclass<int, 5, 1>(m, "KDTiD5L1");
  add_kdt_pyclass<int, 5, 2>(m, "KDTiD5L2");
  add_kdt_pyclass<int, 6, 1>(m, "KDTiD6L1");
  add_kdt_pyclass<int, 6, 2>(m, "KDTiD6L2");
  add_kdt_pyclass<int, 7, 1>(m, "KDTiD7L1");
  add_kdt_pyclass<int, 7, 2>(m, "KDTiD7L2");
  add_kdt_pyclass<int, 8, 1>(m, "KDTiD8L1");
  add_kdt_pyclass<int, 8, 2>(m, "KDTiD8L2");
  add_kdt_pyclass<int, 9, 1>(m, "KDTiD9L1");
  add_kdt_pyclass<int, 9, 2>(m, "KDTiD9L2");
  add_kdt_pyclass<int, 10, 1>(m, "KDTiD10L1");
  add_kdt_pyclass<int, 10, 2>(m, "KDTiD10L2");

}
