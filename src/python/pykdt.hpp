#pragma once

#include <memory>
#include <thread>
#include <utility>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "../napf.hpp"
#include "threadhelper.hpp"

namespace py = pybind11;


template<typename DataT,
         typename DistT,
         typename IndexT,
         int dim,
         unsigned int metric>
using TreeT = typename std::conditional<
    (dim < 4),
    napf::RawPtrTree<DataT, DistT, IndexT, dim, metric>,
    napf::RawPtrHighDimTree<DataT, DistT, IndexT, dim, metric>
  >::type;


template<typename DataT, size_t dim, unsigned int metric>
class PyKDT {
public:

  // let's fix some datatype.
  //   distance is always double, unless DataT is float
  //   index is always unsigned int
  using DistT = typename std::conditional<
    std::is_same<DataT, float>::value,
    float,
    double
  >::type;
  using IndexT = unsigned int;

  using Tree = TreeT<DataT, DistT, IndexT, dim, metric>;
  using Cloud = napf::RawPtrCloud<DataT, IndexT, dim>;


  const int dim_ = dim;
  const unsigned int metric_ = metric;

  py::array_t<DataT> tree_data_;
  DataT* tree_data_ptr_;
  IndexT datalen_ = 0;
  std::unique_ptr<Cloud> cloud_;

  std::unique_ptr<Tree> tree_;

  PyKDT() = default;

  PyKDT(py::array_t<DataT> tree_data) {
    newtree(tree_data);
  }


  /* builds a new tree and saves it as unique_ptr */
  void newtree(py::array_t<DataT> tree_data) {
    // save relevant infos locally
    // don't copy.
    // be aware, this means even if you change tree_data inplace,
    // the tree won't change
    tree_data_ = tree_data;
    const py::buffer_info t_buf = tree_data.request();
    tree_data_ptr_ = static_cast<DataT *>(t_buf.ptr);
    datalen_ = static_cast<IndexT>(t_buf.shape[0]);

    // maybe can check if shape[1] matches dim here

    // prepare cloud and tree
    cloud_ = std::unique_ptr<Cloud>(
      new Cloud(tree_data_ptr_, static_cast<IndexT>(t_buf.size))
    );
    tree_ = std::unique_ptr<Tree>(new Tree(dim, *cloud_));

  }

  /* given query points, returns indices and distances*/ 
  py::tuple knn_search(const py::array_t<DataT> qpts,
                       const int kneighbors,
                       const int nthread) {

    // in
    const py::buffer_info q_buf = qpts.request();
    const DataT* q_buf_ptr = static_cast<DataT *>(q_buf.ptr);
    const int qlen = q_buf.shape[0];

    // out
    py::array_t<IndexT> indices(qlen * kneighbors);
    py::buffer_info i_buf = indices.request();
    IndexT* i_buf_ptr = static_cast<IndexT *>(i_buf.ptr);
    py::array_t<DistT> dist(qlen * kneighbors);
    py::buffer_info d_buf = dist.request();
    DistT* d_buf_ptr = static_cast<DistT *>(d_buf.ptr);

    if (kneighbors > static_cast<int>(datalen_)) {
      std::cout << "WARNING - "
                << "kneighbors (" << kneighbors
                << ") is bigger than number of tree data (" << datalen_
                << "! "
                << "Returning arrays `[:, " << datalen_ - kneighbors
                << ":]` entries will be filled with random indices."
                << std::endl;

    }

    // prepare routine in lambda so that it can be executed with nthreads
    auto searchknn = [&] (int begin, int end) {
      for (int i{begin}; i < end; i++) {
        const int j{i * static_cast<int>(dim)};
        const int k{i * kneighbors};
        tree_->knnSearch(&q_buf_ptr[j],
                         kneighbors,
                         &i_buf_ptr[k],
                         &d_buf_ptr[k]);
      }
    };

    // don't worry, if nthread == 1, we don't create threads.
    nthread_execution(searchknn, qlen, nthread);

    indices = indices.reshape({qlen, kneighbors});
    dist = dist.reshape({qlen, kneighbors});

    return py::make_tuple(indices, dist);
  }


  /* scipy KDTree style query */
  py::tuple query(const py::array_t<DataT> qpts,
                  const int nthread) {
    return knn_search(qpts, 1, nthread); 
  }

  /* radius search */
  py::tuple radius_search(const py::array_t<DataT> qpts,
                          const DistT radius,
                          const bool return_sorted,
                          const int nthread) {
    // in
    const py::buffer_info q_buf = qpts.request();
    const DataT* q_buf_ptr = static_cast<DataT *>(q_buf.ptr);
    const int qlen = q_buf.shape[0];

    // out
    py::list indices;
    py::list dist;

    auto searchradius = [&] (int begin, int end) {
      for (int i{begin}; i < end; i++) {
        // prepare input
        std::vector<std::pair<IndexT, DistT>> matches;
        nanoflann::SearchParams params;
        params.sorted = return_sorted;

        const int j{i * static_cast<int>(dim)};
        // call
        const auto nmatches = tree_->radiusSearch(&q_buf_ptr[j],
                                                  radius,
                                                  matches,
                                                  params);

        // prepare output
        py::array_t<IndexT> ids(nmatches);
        py::buffer_info i_buf = ids.request();
        IndexT* i_buf_ptr = static_cast<IndexT *>(i_buf.ptr);

        py::array_t<DistT> ds(nmatches);
        py::buffer_info d_buf = ds.request();
        DistT* d_buf_ptr = static_cast<DistT *>(d_buf.ptr);

        // unpack and fill output
        for (int i{0}; i < (int) nmatches; i++) {
          i_buf_ptr[i] = matches[i].first;
          d_buf_ptr[i] = matches[i].second;
        }

        // append to return list
        indices.append(ids);
        dist.append(ds);
      }
    };

    nthread_execution(searchradius, qlen, nthread);

    return py::make_tuple(indices, dist);
  }

  /* radii search. in other words, each query can have different radius */
  py::tuple radii_search(const py::array_t<DataT> qpts,
                         const py::array_t<DistT> radii,
                         const bool return_sorted,
                         const int nthread) {
    // in
    const py::buffer_info q_buf = qpts.request();
    const DataT* q_buf_ptr = static_cast<DataT*>(q_buf.ptr);
    const int qlen = q_buf.shape[0];

    const py::buffer_info r_buf = radii.request();
    const DistT* r_buf_ptr = static_cast<DistT *>(r_buf.ptr);
    const int rlen = r_buf.shape[0];

    // execution ending error is too brutal and merciless
    // print warning and return empty
    if (qlen != rlen) {
      std::cout << "CRITICAL WARNING - "
                << "query length (" << qlen
                << ") and radii length (" << rlen
                << ") differ! "
                << "returning empty tuple."
                << std::endl;

      return py::tuple{};
    }

    // out
    py::list indices;
    py::list dist;

    auto searchradius = [&] (int begin, int end) {
      for (int i{begin}; i < end; i++) {
        // prepare input
        std::vector<std::pair<IndexT, DistT>> matches;
        nanoflann::SearchParams params;
        params.sorted = return_sorted; 

        const int j{i * static_cast<int>(dim)};
        // call
        const auto nmatches = tree_->radiusSearch(&q_buf_ptr[j],
                                                  r_buf_ptr[i],
                                                  matches,
                                                  params);

        // prepare output
        // potentially could be replaced with list.
        py::array_t<IndexT> ids(nmatches);
        py::buffer_info i_buf = ids.request();
        IndexT* i_buf_ptr = static_cast<IndexT *>(i_buf.ptr);

        py::array_t<DistT> ds(nmatches);
        py::buffer_info d_buf = ds.request();
        DistT* d_buf_ptr = static_cast<DistT *>(d_buf.ptr);

        // unpack and fill output
        for (int i{0}; i < (int) nmatches; i++) {
          i_buf_ptr[i] = matches[i].first;
          d_buf_ptr[i] = matches[i].second;
        }

        // append to return list
        indices.append(ids);
        dist.append(ds);
      }
    };

    nthread_execution(searchradius, qlen, nthread);

    return py::make_tuple(indices, dist);
  }
                    
};


template<typename T, int dim, unsigned int metric>
void add_kdt_pyclass(py::module& m, const char *class_name) {
  using KDT = PyKDT<T, dim, metric>;

  py::class_<KDT> klasse(m, class_name);

  klasse.def(py::init<>())
        .def(py::init<py::array_t<T>>(),
                 py::arg("tree_data"))
        .def_readonly("tree_data",
                           &KDT::tree_data_)
        .def_readonly("dim",
                           &KDT::dim_)
        .def_readonly("metric",
                           &KDT::metric_)
        .def("newtree",
                 &KDT::newtree,
                 py::arg("tree_data"))
        .def("knn_search",
                 &KDT::knn_search,
                 py::arg("queries"),
                 py::arg("kneighbors"),
                 py::arg("nthread"))
        .def("query",
                 &KDT::query,
                 py::arg("queries"),
                 py::arg("nthread"))
        .def("radius_search",
                 &KDT::radius_search,
                 py::arg("queries"),
                 py::arg("radius"),
                 py::arg("return_sorted"),
                 py::arg("nthread"))
        .def("radii_search",
                 &KDT::radii_search,
                 py::arg("queries"),
                 py::arg("radii"),
                 py::arg("return_sorted"),
                 py::arg("nthread"))
        ;

}
