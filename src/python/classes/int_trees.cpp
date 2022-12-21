#include "../pykdt.hpp"

void init_int_trees(py::module_& m) {
  add_kdt_pyclass<int32_t, 1, 1>(m, "KDTiD1L1");
  add_kdt_pyclass<int32_t, 1, 2>(m, "KDTiD1L2");
  add_kdt_pyclass<int32_t, 2, 1>(m, "KDTiD2L1");
  add_kdt_pyclass<int32_t, 2, 2>(m, "KDTiD2L2");
  add_kdt_pyclass<int32_t, 3, 1>(m, "KDTiD3L1");
  add_kdt_pyclass<int32_t, 3, 2>(m, "KDTiD3L2");
  add_kdt_pyclass<int32_t, 4, 1>(m, "KDTiD4L1");
  add_kdt_pyclass<int32_t, 4, 2>(m, "KDTiD4L2");
  add_kdt_pyclass<int32_t, 5, 1>(m, "KDTiD5L1");
  add_kdt_pyclass<int32_t, 5, 2>(m, "KDTiD5L2");
  add_kdt_pyclass<int32_t, 6, 1>(m, "KDTiD6L1");
  add_kdt_pyclass<int32_t, 6, 2>(m, "KDTiD6L2");
  add_kdt_pyclass<int32_t, 7, 1>(m, "KDTiD7L1");
  add_kdt_pyclass<int32_t, 7, 2>(m, "KDTiD7L2");
  add_kdt_pyclass<int32_t, 8, 1>(m, "KDTiD8L1");
  add_kdt_pyclass<int32_t, 8, 2>(m, "KDTiD8L2");
  add_kdt_pyclass<int32_t, 9, 1>(m, "KDTiD9L1");
  add_kdt_pyclass<int32_t, 9, 2>(m, "KDTiD9L2");
  add_kdt_pyclass<int32_t, 10, 1>(m, "KDTiD10L1");
  add_kdt_pyclass<int32_t, 10, 2>(m, "KDTiD10L2");
}
