#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_int_trees(py::module_&);
void init_double_trees(py::module_&);

PYBIND11_MODULE(napf, m) {
  init_int_trees(m);
  init_double_trees(m);
}
