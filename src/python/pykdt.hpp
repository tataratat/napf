#pragma once

#include <memory>
#include <thread>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "../napf.hpp"
#include "threadhelper.hpp"

namespace py = pybind11;

template<typename T, size_t dim, unsigned int metric>
  using TreeT = std::conditional_t<
    (dim < 4),
    napf::RawPtrTree<T, dim, metric>,
    napf::RawPtrHighDimTree<T, dim, metric>
  >;


templace<typename T, size_t dim, unsigned int metric>
class KDT {
public:


  const dim_ = dim;
  const metric_ = metric;

  T* tree_data_;
  size_t datalen_ = 0;
  napf::RawPtrCloud cloud_;

  std::unique_ptr<TreeT> tree_;
  std::unique_ptr<TreeT> hightree_;

  KDT() = default;

  KDT(py::array_t<T> tree_data) {
    newtree(tree_data);
  }


  /* builds a new tree and saves it as unique_ptr */
  void newtree(py::array_t<T> tree_data) {
    // don't copy.
    const py::buffer_info t_buf = tree_data.request();

    // save relevant infos locally
    tree_data_ = static_cast<T *>(t_buf.ptr);
    datalen_ = t_buf.shape[0];

    // maybe can check if shape[1] matches dim here

    // prepare cloud and tree
    cloud_ = std::make_unique<TreeT<T, dim, metric>>(
        tree_data_,
        datalen_);
    tree_ = std::make_unique<TreeT<T, dim, metric>>(dim, cloud_);

    // build tree
    tree_->buildIndex();

  }

  /* given query points, returns */ 
  py::tuple knn_search(const py::array_t<T> qpts,
                       const int kneighbors,
                       const int nthread) {

    // in
    const py::buffer_info q_buf = qpts.request();
    const double* q_buf_ptr = static_cast<double *>(q_buf.ptr);
    const int qlen = q_buf.shape[0];

    // out
    py::array_t<int> indices(qlen * kneighbors);
    py::buffer_info i_buf = indices.request();
    int* i_buf_ptr = static_cast<int *>(i_buf.ptr);
    py::array_t<double> dist(qlen * kneighbors);
    py::buffer_info d_buf = dist.request();
    double* d_buf_ptr = static_cast<double *>(d_buf.ptr);

    // prepare routine in lambda so that it can be executed with nthreads
    auto searchknn = [&] (int begin, int end) {
      for (int i{begin}; i < end; i++) {
       tree_->knnSearch(&q_buf_ptr[begin],
                        kneighbors,
                        &i_buf_ptr[begin],
                        &d_buf_ptr[begin]);
      }
    }

    // nthread exe
    nthread_execution(searchknn, qlen, nthread);

    indices.resize({qlen, kneighbors});
    dist.resize({qlen, kneighbors});

    return py::make_tuple(indices, dist);
  }
}
