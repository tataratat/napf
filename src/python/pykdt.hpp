#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

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
    napf::RawPtrHighDimTree<DataT, DistT, IndexT, dim, metric>>::type;

template<typename DataT, size_t dim, unsigned int metric>
class PyKDT {
public:
  // let's fix some datatype.
  //   distance is always double, unless DataT is float
  //   index is always unsigned int
  using DistT = typename std::
      conditional<std::is_same<DataT, float>::value, float, double>::type;
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

  PyKDT(py::array_t<DataT> tree_data) { newtree(tree_data); }

  /* builds a new tree and saves it as unique_ptr */
  void newtree(py::array_t<DataT> tree_data) {
    // save relevant infos locally
    // don't copy.
    // be aware, this means even if you change tree_data inplace,
    // the tree won't change
    tree_data_ = tree_data;
    const py::buffer_info t_buf = tree_data.request();
    tree_data_ptr_ = static_cast<DataT*>(t_buf.ptr);
    datalen_ = static_cast<IndexT>(t_buf.shape[0]);

    // maybe can check if shape[1] matches dim here

    // prepare cloud and tree
    cloud_ = std::unique_ptr<Cloud>(
        new Cloud(tree_data_ptr_, static_cast<IndexT>(t_buf.size)));
    tree_ = std::unique_ptr<Tree>(new Tree(dim, *cloud_));
  }

  /* given query points, returns indices and distances */
  py::tuple knn_search(const py::array_t<DataT> qpts,
                       const int kneighbors,
                       const int nthread) {

    // in
    const py::buffer_info q_buf = qpts.request();
    const DataT* q_buf_ptr = static_cast<DataT*>(q_buf.ptr);
    const int qlen = q_buf.shape[0];

    // out
    py::array_t<IndexT> indices(qlen * kneighbors);
    py::buffer_info i_buf = indices.request();
    IndexT* i_buf_ptr = static_cast<IndexT*>(i_buf.ptr);
    py::array_t<DistT> dist(qlen * kneighbors);
    py::buffer_info d_buf = dist.request();
    DistT* d_buf_ptr = static_cast<DistT*>(d_buf.ptr);

    if (kneighbors > static_cast<int>(datalen_)) {
      std::cout << "WARNING - "
                << "kneighbors (" << kneighbors
                << ") is bigger than number of tree data (" << datalen_ << "! "
                << "Returning arrays `[:, " << datalen_ - kneighbors
                << ":]` entries will be filled with random indices."
                << std::endl;
    }

    // prepare routine in lambda so that it can be executed with nthreads
    auto searchknn = [&](int begin, int end) {
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

    indices.resize({qlen, kneighbors});
    dist.resize({qlen, kneighbors});

    return py::make_tuple(dist, indices);
  }

  /* scipy KDTree style query */
  py::tuple query(const py::array_t<DataT> qpts, const int nthread) {
    return knn_search(qpts, 1, nthread);
  }

  /* radius search */
  py::tuple radius_search(const py::array_t<DataT> qpts,
                          const DistT radius,
                          const bool return_sorted,
                          const int nthread) {
    // in
    const py::buffer_info q_buf = qpts.request();
    const DataT* q_buf_ptr = static_cast<DataT*>(q_buf.ptr);
    const int qlen = q_buf.shape[0];
    nanoflann::SearchParameters params;
    params.sorted = return_sorted;

    // out
    py::list out_indices{}, out_dist{};
    std::vector<py::list> indices(nthread);
    std::vector<py::list> dist(nthread);
    const int chunk_size = (qlen + nthread - 1) / nthread;

    auto searchradius = [&](int start, int) {
      const int start_index = start * chunk_size;
      const int end_index = std::min((start + 1) * chunk_size, qlen);
      const int n_queries = end_index - start_index;

      auto& this_indices = indices[start];
      this_indices = py::list(n_queries);
      auto& this_dist = dist[start];
      this_dist = py::list(n_queries);

      int l{}; // for list element id
      for (int i{start_index}; i < end_index; i++) {
        // prepare input
        std::vector<nanoflann::ResultItem<IndexT, DistT>> matches;

        const int j{i * static_cast<int>(dim)};
        // call
        const auto nmatches =
            tree_->radiusSearch(&q_buf_ptr[j], radius, matches, params);

        // prepare output
        py::array_t<IndexT> ids(nmatches);
        py::buffer_info i_buf = ids.request();
        IndexT* i_buf_ptr = static_cast<IndexT*>(i_buf.ptr);

        py::array_t<DistT> ds(nmatches);
        py::buffer_info d_buf = ds.request();
        DistT* d_buf_ptr = static_cast<DistT*>(d_buf.ptr);

        // unpack and fill output
        for (int k{}; k < (int) nmatches; ++k) {
          i_buf_ptr[k] = matches[k].first;
          d_buf_ptr[k] = matches[k].second;
        }

        this_indices[l] = ids;
        this_dist[l] = ds;
        ++l;
      }
    };

    nthread_execution(searchradius, nthread, nthread);

    for (int i{}; i < nthread; ++i) {
      out_indices += indices[i];
      out_dist += dist[i];
    }

    return py::make_tuple(out_indices, out_dist);
  }

  /// @brief
  /// @param qpts
  /// @param radius
  /// @param return_sorted here, sort is based on ids, not distance
  /// @param nthread
  /// @return
  py::list query_ball_point(const py::array_t<DataT> qpts,
                            const DistT radius,
                            const bool return_sorted,
                            const int nthread) {

    using PairType = nanoflann::ResultItem<IndexT, DistT>;

    // in
    const py::buffer_info q_buf = qpts.request();
    const DataT* q_buf_ptr = static_cast<DataT*>(q_buf.ptr);
    const int qlen = q_buf.shape[0];
    // we don't need distance based sorting
    nanoflann::SearchParameters params;
    params.sorted = false;

    // out
    py::list out_indices{};
    std::vector<py::list> indices(nthread);
    const int chunk_size = (qlen + nthread - 1) / nthread;

    auto searchradius = [&](int start, int) {
      const int start_index = start * chunk_size;
      const int end_index = std::min((start + 1) * chunk_size, qlen);
      const int n_queries = end_index - start_index;

      auto& this_indices = indices[start];
      this_indices = py::list(n_queries);

      int l{}; // for list element id
      for (int i{start_index}; i < end_index; i++) {
        // prepare input
        std::vector<PairType> matches;

        const int j{i * static_cast<int>(dim)};
        // call
        const auto nmatches =
            tree_->radiusSearch(&q_buf_ptr[j], radius, matches, params);

        // prepare output
        py::array_t<IndexT> ids(nmatches);
        py::buffer_info i_buf = ids.request();
        IndexT* i_buf_ptr = static_cast<IndexT*>(i_buf.ptr);

        // sort ids
        if (return_sorted) {
          std::sort(matches.begin(),
                    matches.end(),
                    [](const PairType& a, const PairType& b) {
                      return a.first < b.first;
                    });
        }

        // unpack and fill output
        for (int k{}; k < (int) nmatches; ++k) {
          i_buf_ptr[k] = matches[k].first;
        }

        this_indices[l] = ids;
        ++l;
      }
    };

    nthread_execution(searchradius, nthread, nthread);

    for (auto& ind : indices) {
      out_indices += ind;
    }

    return out_indices;
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
    const DistT* r_buf_ptr = static_cast<DistT*>(r_buf.ptr);
    const int rlen = r_buf.shape[0];

    // execution ending error is too brutal and merciless
    // print warning and return empty
    if (qlen != rlen) {
      std::cout << "CRITICAL WARNING - "
                << "query length (" << qlen << ") and radii length (" << rlen
                << ") differ! "
                << "returning empty tuple." << std::endl;

      return py::tuple{};
    }

    nanoflann::SearchParameters params;
    params.sorted = return_sorted;

    // out
    py::list out_indices{}, out_dist{};
    std::vector<py::list> indices(nthread);
    std::vector<py::list> dist(nthread);
    const int chunk_size = (qlen + nthread - 1) / nthread;

    auto searchradius = [&](int start, int) {
      const int start_index = start * chunk_size;
      const int end_index = std::min((start + 1) * chunk_size, qlen);
      const int n_queries = end_index - start_index;

      auto& this_indices = indices[start];
      this_indices = py::list(n_queries);
      auto& this_dist = dist[start];
      this_dist = py::list(n_queries);

      int l{};
      for (int i{start_index}; i < end_index; i++) {
        // prepare input
        std::vector<nanoflann::ResultItem<IndexT, DistT>> matches;

        const int j{i * static_cast<int>(dim)};
        // call
        const auto nmatches =
            tree_->radiusSearch(&q_buf_ptr[j], r_buf_ptr[i], matches, params);

        // prepare output
        // potentially could be replaced with list.
        py::array_t<IndexT> ids(nmatches);
        py::buffer_info i_buf = ids.request();
        IndexT* i_buf_ptr = static_cast<IndexT*>(i_buf.ptr);

        py::array_t<DistT> ds(nmatches);
        py::buffer_info d_buf = ds.request();
        DistT* d_buf_ptr = static_cast<DistT*>(d_buf.ptr);

        // unpack and fill output
        for (int k{0}; k < (int) nmatches; ++k) {
          i_buf_ptr[k] = matches[k].first;
          d_buf_ptr[k] = matches[k].second;
        }

        this_indices[l] = ids;
        this_dist[l] = ds;
        ++l;
      }
    };

    nthread_execution(searchradius, nthread, nthread);

    for (int i{}; i < nthread; ++i) {
      out_indices += indices[i];
      out_dist += dist[i];
    }

    return py::make_tuple(indices, dist);
  }
};

template<typename T, int dim, unsigned int metric>
void add_kdt_pyclass(py::module& m, const char* class_name) {
  using KDT = PyKDT<T, dim, metric>;

  py::class_<KDT> klasse(m, class_name);

  klasse.def(py::init<>())
      .def(py::init<py::array_t<T>>(), py::arg("tree_data"))
      .def_readonly("tree_data", &KDT::tree_data_)
      .def_readonly("dim", &KDT::dim_)
      .def_readonly("metric", &KDT::metric_)
      .def("newtree", &KDT::newtree, py::arg("tree_data"))
      .def("knn_search",
           &KDT::knn_search,
           py::arg("queries"),
           py::arg("kneighbors"),
           py::arg("nthread"))
      .def("query", &KDT::query, py::arg("queries"), py::arg("nthread"))
      .def("radius_search",
           &KDT::radius_search,
           py::arg("queries"),
           py::arg("radius"),
           py::arg("return_sorted"),
           py::arg("nthread"))
      .def("query_ball_point",
           &KDT::query_ball_point,
           py::arg("queries"),
           py::arg("radius"),
           py::arg("return_sorted"),
           py::arg("nthread"))
      .def("radii_search",
           &KDT::radii_search,
           py::arg("queries"),
           py::arg("radii"),
           py::arg("return_sorted"),
           py::arg("nthread"));
}
