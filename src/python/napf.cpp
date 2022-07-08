#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_int_trees(py::module_&);
void init_long_trees(py::module_&);
void init_float_trees(py::module_&);
void init_double_trees(py::module_&);

PYBIND11_MODULE(_napf, m) {
  init_int_trees(m);
  init_long_trees(m);
  init_float_trees(m);
  init_double_trees(m);
}
