#include "../pykdt.hpp"

namespace napf {

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
  add_kdt_pyclass<int32_t, 11, 1>(m, "KDTiD11L1");
  add_kdt_pyclass<int32_t, 11, 2>(m, "KDTiD11L2");
  add_kdt_pyclass<int32_t, 12, 1>(m, "KDTiD12L1");
  add_kdt_pyclass<int32_t, 12, 2>(m, "KDTiD12L2");
  add_kdt_pyclass<int32_t, 13, 1>(m, "KDTiD13L1");
  add_kdt_pyclass<int32_t, 13, 2>(m, "KDTiD13L2");
  add_kdt_pyclass<int32_t, 14, 1>(m, "KDTiD14L1");
  add_kdt_pyclass<int32_t, 14, 2>(m, "KDTiD14L2");
  add_kdt_pyclass<int32_t, 15, 1>(m, "KDTiD15L1");
  add_kdt_pyclass<int32_t, 15, 2>(m, "KDTiD15L2");
  add_kdt_pyclass<int32_t, 16, 1>(m, "KDTiD16L1");
  add_kdt_pyclass<int32_t, 16, 2>(m, "KDTiD16L2");
  add_kdt_pyclass<int32_t, 17, 1>(m, "KDTiD17L1");
  add_kdt_pyclass<int32_t, 17, 2>(m, "KDTiD17L2");
  add_kdt_pyclass<int32_t, 18, 1>(m, "KDTiD18L1");
  add_kdt_pyclass<int32_t, 18, 2>(m, "KDTiD18L2");
  add_kdt_pyclass<int32_t, 19, 1>(m, "KDTiD19L1");
  add_kdt_pyclass<int32_t, 19, 2>(m, "KDTiD19L2");
  add_kdt_pyclass<int32_t, 20, 1>(m, "KDTiD20L1");
  add_kdt_pyclass<int32_t, 20, 2>(m, "KDTiD20L2");
}

} // namespace napf
