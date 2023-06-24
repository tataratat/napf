#include "../pykdt.hpp"

#include <pybind11/stl_bind.h>

namespace napf {

namespace py = pybind11;

void init_radius_search_result_vector(py::module_& m) {
  py::bind_vector<FloatVector>(m, "FloatVector");
  py::bind_vector<FloatVectorVector>(m, "FloatVectorVector");
  py::bind_vector<DoubleVector>(m, "DoubleVector");
  py::bind_vector<DoubleVectorVector>(m, "DoubleVectorVector");
  py::bind_vector<UIntVector>(m, "UIntVector");
  py::bind_vector<UIntVectorVector>(m, "UIntVectorVector");
}

} // namespace napf
