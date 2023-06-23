#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace napf {

void init_int_trees(py::module_&);
void init_long_trees(py::module_&);
void init_float_trees(py::module_&);
void init_double_trees(py::module_&);
void init_radius_search_result_vector(py::module_&);

} // namespace napf

PYBIND11_MODULE(_napf, m) {
  napf::init_int_trees(m);
  napf::init_long_trees(m);
  napf::init_float_trees(m);
  napf::init_double_trees(m);
  napf::init_radius_search_result_vector(m);
}
