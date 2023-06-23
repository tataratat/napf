#include "../pykdt.hpp"

namespace napf {

void init_long_trees(py::module_& m) {
  add_kdt_pyclass<int64_t, 1, 1>(m, "KDTlD1L1");
  add_kdt_pyclass<int64_t, 1, 2>(m, "KDTlD1L2");
  add_kdt_pyclass<int64_t, 2, 1>(m, "KDTlD2L1");
  add_kdt_pyclass<int64_t, 2, 2>(m, "KDTlD2L2");
  add_kdt_pyclass<int64_t, 3, 1>(m, "KDTlD3L1");
  add_kdt_pyclass<int64_t, 3, 2>(m, "KDTlD3L2");
  add_kdt_pyclass<int64_t, 4, 1>(m, "KDTlD4L1");
  add_kdt_pyclass<int64_t, 4, 2>(m, "KDTlD4L2");
  add_kdt_pyclass<int64_t, 5, 1>(m, "KDTlD5L1");
  add_kdt_pyclass<int64_t, 5, 2>(m, "KDTlD5L2");
  add_kdt_pyclass<int64_t, 6, 1>(m, "KDTlD6L1");
  add_kdt_pyclass<int64_t, 6, 2>(m, "KDTlD6L2");
  add_kdt_pyclass<int64_t, 7, 1>(m, "KDTlD7L1");
  add_kdt_pyclass<int64_t, 7, 2>(m, "KDTlD7L2");
  add_kdt_pyclass<int64_t, 8, 1>(m, "KDTlD8L1");
  add_kdt_pyclass<int64_t, 8, 2>(m, "KDTlD8L2");
  add_kdt_pyclass<int64_t, 9, 1>(m, "KDTlD9L1");
  add_kdt_pyclass<int64_t, 9, 2>(m, "KDTlD9L2");
  add_kdt_pyclass<int64_t, 10, 1>(m, "KDTlD10L1");
  add_kdt_pyclass<int64_t, 10, 2>(m, "KDTlD10L2");
  add_kdt_pyclass<int64_t, 11, 1>(m, "KDTlD11L1");
  add_kdt_pyclass<int64_t, 11, 2>(m, "KDTlD11L2");
  add_kdt_pyclass<int64_t, 12, 1>(m, "KDTlD12L1");
  add_kdt_pyclass<int64_t, 12, 2>(m, "KDTlD12L2");
  add_kdt_pyclass<int64_t, 13, 1>(m, "KDTlD13L1");
  add_kdt_pyclass<int64_t, 13, 2>(m, "KDTlD13L2");
  add_kdt_pyclass<int64_t, 14, 1>(m, "KDTlD14L1");
  add_kdt_pyclass<int64_t, 14, 2>(m, "KDTlD14L2");
  add_kdt_pyclass<int64_t, 15, 1>(m, "KDTlD15L1");
  add_kdt_pyclass<int64_t, 15, 2>(m, "KDTlD15L2");
  add_kdt_pyclass<int64_t, 16, 1>(m, "KDTlD16L1");
  add_kdt_pyclass<int64_t, 16, 2>(m, "KDTlD16L2");
  add_kdt_pyclass<int64_t, 17, 1>(m, "KDTlD17L1");
  add_kdt_pyclass<int64_t, 17, 2>(m, "KDTlD17L2");
  add_kdt_pyclass<int64_t, 18, 1>(m, "KDTlD18L1");
  add_kdt_pyclass<int64_t, 18, 2>(m, "KDTlD18L2");
  add_kdt_pyclass<int64_t, 19, 1>(m, "KDTlD19L1");
  add_kdt_pyclass<int64_t, 19, 2>(m, "KDTlD19L2");
  add_kdt_pyclass<int64_t, 20, 1>(m, "KDTlD20L1");
  add_kdt_pyclass<int64_t, 20, 2>(m, "KDTlD20L2");
}

} // namespace napf
