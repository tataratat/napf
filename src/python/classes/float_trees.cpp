#include "../pykdt.hpp"

namespace napf {

void init_float_trees(py::module_& m) {
  add_kdt_pyclass<float, 1>(m, "KDTfL1");
  add_kdt_pyclass<float, 2>(m, "KDTfL2");
}

} // namespace napf
