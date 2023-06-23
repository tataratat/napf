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

namespace napf {

namespace py = pybind11;

// Radius search result holder types
using FloatVector = std::vector<float>;
using FloatVectorVector = std::vector<FloatVector>;
using DoubleVector = std::vector<double>;
using DoubleVectorVector = std::vector<DoubleVector>;
using UIntVector = std::vector<unsigned int>;
using UIntVectorVector = std::vector<UIntVector>;

// fix index type and add alias
using IndexType = typename UIntVector::value_type;
using IndexVector = UIntVector;
using IndexVectorVector = UIntVectorVector;

template<typename DataT,
         typename DistT,
         typename IndexT,
         int dim,
         unsigned int metric>
using TreeType = typename std::conditional<
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
  using DistVector =
      typename std::conditional<std::is_same<DistT, float>::value,
                                FloatVector,
                                DoubleVector>::type;
  using DistVectorVector =
      typename std::conditional<std::is_same<DistVector, FloatVector>::value,
                                FloatVectorVector,
                                DoubleVectorVector>::type;

  using Tree = TreeType<DataT, DistT, IndexType, dim, metric>;
  using Cloud = napf::RawPtrCloud<DataT, IndexType, dim>;

  const int dim_ = dim;
  const unsigned int metric_ = metric;

  py::array_t<DataT> tree_data_;
  DataT* tree_data_ptr_;
  IndexType datalen_ = 0;
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
    datalen_ = static_cast<IndexType>(t_buf.shape[0]);

    // maybe can check if shape[1] matches dim here

    // prepare cloud and tree
    cloud_ = std::unique_ptr<Cloud>(
        new Cloud(tree_data_ptr_, static_cast<IndexType>(t_buf.size)));
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
    py::array_t<IndexType> indices(qlen * kneighbors);
    py::buffer_info i_buf = indices.request();
    IndexType* i_buf_ptr = static_cast<IndexType*>(i_buf.ptr);
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
    IndexVectorVector out_indices(qlen);
    DistVectorVector out_dist(qlen);

    auto searchradius = [&](int begin, int end) {
      for (int i{begin}; i < end; i++) {

        auto& this_indices = out_indices[i];
        auto& this_dist = out_dist[i];

        // prepare input
        std::vector<nanoflann::ResultItem<IndexType, DistT>> matches;

        // call
        const auto nmatches =
            tree_->radiusSearch(&q_buf_ptr[i * dim_], radius, matches, params);

        this_indices.reserve(nmatches);
        this_dist.reserve(nmatches);

        for (auto& match : matches) {
          this_indices.emplace_back(match.first);
          this_dist.emplace_back(match.second);
        }
      }
    };

    nthread_execution(searchradius, qlen, nthread);

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

    using PairType = nanoflann::ResultItem<IndexType, DistT>;

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

        // call
        const auto nmatches =
            tree_->radiusSearch(&q_buf_ptr[i * dim_], radius, matches, params);

        // prepare output
        py::array_t<IndexType> ids(nmatches);
        py::buffer_info i_buf = ids.request();
        IndexType* i_buf_ptr = static_cast<IndexType*>(i_buf.ptr);

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
    IndexVectorVector out_indices(qlen);
    DistVectorVector out_dist(qlen);

    auto searchradius = [&](int start, int end) {
      auto& this_indices = out_indices[start];
      auto& this_dist = out_dist[start];

      for (int i{start}; i < end; ++i) {
        // prepare input
        std::vector<nanoflann::ResultItem<IndexType, DistT>> matches;

        // call
        const auto nmatches = tree_->radiusSearch(&q_buf_ptr[i * dim_],
                                                  r_buf_ptr[i],
                                                  matches,
                                                  params);

        this_indices.reserve(nmatches);
        this_dist.reserve(nmatches);

        // unpack and fill output
        for (int k{0}; k < (int) nmatches; ++k) {
          this_indices.emplace_back(matches[k].first);
          this_dist.emplace_back(matches[k].second);
        }
      }
    };

    nthread_execution(searchradius, qlen, nthread);

    return py::make_tuple(out_indices, out_dist);
  }
};

template<typename T, int dim, unsigned int metric>
void add_kdt_pyclass(py::module_& m, const char* class_name) {
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

} // namespace napf
