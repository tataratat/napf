#include "../pykdt.hpp"

namespace napf {

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
  add_kdt_pyclass<double, 11, 1>(m, "KDTdD11L1");
  add_kdt_pyclass<double, 11, 2>(m, "KDTdD11L2");
  add_kdt_pyclass<double, 12, 1>(m, "KDTdD12L1");
  add_kdt_pyclass<double, 12, 2>(m, "KDTdD12L2");
  add_kdt_pyclass<double, 13, 1>(m, "KDTdD13L1");
  add_kdt_pyclass<double, 13, 2>(m, "KDTdD13L2");
  add_kdt_pyclass<double, 14, 1>(m, "KDTdD14L1");
  add_kdt_pyclass<double, 14, 2>(m, "KDTdD14L2");
  add_kdt_pyclass<double, 15, 1>(m, "KDTdD15L1");
  add_kdt_pyclass<double, 15, 2>(m, "KDTdD15L2");
  add_kdt_pyclass<double, 16, 1>(m, "KDTdD16L1");
  add_kdt_pyclass<double, 16, 2>(m, "KDTdD16L2");
  add_kdt_pyclass<double, 17, 1>(m, "KDTdD17L1");
  add_kdt_pyclass<double, 17, 2>(m, "KDTdD17L2");
  add_kdt_pyclass<double, 18, 1>(m, "KDTdD18L1");
  add_kdt_pyclass<double, 18, 2>(m, "KDTdD18L2");
  add_kdt_pyclass<double, 19, 1>(m, "KDTdD19L1");
  add_kdt_pyclass<double, 19, 2>(m, "KDTdD19L2");
  add_kdt_pyclass<double, 20, 1>(m, "KDTdD20L1");
  add_kdt_pyclass<double, 20, 2>(m, "KDTdD20L2");
}

} // namespace napf
