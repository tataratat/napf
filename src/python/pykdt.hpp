#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
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

template<typename DataT, unsigned int metric>
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

  using Tree = ArrayTree<DataT, DistT, IndexType, metric>;
  using Cloud = napf::ArrayCloud<DataT, IndexType>;

  int dim_{};
  const unsigned int metric_ = metric;
  size_t leaf_size_{10};
  int nthread_{1};

  py::array_t<DataT> tree_data_;
  DataT* tree_data_ptr_ = nullptr;
  IndexType datalen_ = 0;
  std::unique_ptr<Cloud> cloud_;
  std::unique_ptr<Tree> tree_;

  PyKDT() = default;

  PyKDT(PyKDT&& other) noexcept = default;

  PyKDT(py::array_t<DataT> tree_data) { newtree(tree_data, 10, 1); }
  PyKDT(py::array_t<DataT> tree_data,
        const size_t leaf_size,
        const int nthread) {
    newtree(tree_data, leaf_size, nthread);
  }

  /* builds a new tree and saves it as unique_ptr */
  void newtree(py::array_t<DataT> tree_data,
               const size_t leaf_size = 10,
               const int nthread = 1) {
    // save settings
    dim_ = tree_data.shape(1);
    leaf_size_ = leaf_size;
    nthread_ = nthread;

    // create build param
    nanoflann::KDTreeSingleIndexAdaptorParams params(
        leaf_size,
        nanoflann::KDTreeSingleIndexAdaptorFlags::None,
        static_cast<unsigned int>(nthread));

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
        new Cloud(tree_data_ptr_, static_cast<IndexType>(t_buf.size), dim_));
    tree_ = std::unique_ptr<Tree>(new Tree(dim_, *cloud_, params));
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
    auto searchknn = [&](int begin, int end, int) {
      for (int i{begin}; i < end; i++) {
        const int j{i * dim_};
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

    auto searchradius = [&](int begin, int end, int) {
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

    return py::make_tuple<py::return_value_policy::move>(out_indices, out_dist);
  }

  /// @brief
  /// @param qpts
  /// @param radius
  /// @param return_sorted here, sort is based on ids, not distance
  /// @param nthread
  /// @return
  IndexVectorVector query_ball_point(const py::array_t<DataT> qpts,
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
    IndexVectorVector out_indices(qlen);

    auto searchradius = [&](int start, int end, int) {
      for (int i{start}; i < end; i++) {
        auto& this_indices = out_indices[i];

        // prepare input
        std::vector<PairType> matches;

        // call
        const auto nmatches =
            tree_->radiusSearch(&q_buf_ptr[i * dim_], radius, matches, params);

        // prepare output
        this_indices.reserve(nmatches);

        // unpack and fill output
        for (auto& match : matches) {
          this_indices.emplace_back(match.first);
        }

        // sort ids
        if (return_sorted) {
          // default sort is good here.
          std::sort(this_indices.begin(), this_indices.end());
        }
      }
    };

    nthread_execution(searchradius, qlen, nthread);

    return out_indices;
  }

  /// @brief unique points, indices of unique points, inverse indices to create
  /// original points base on unique points.
  /// @param radius
  /// @param return_intersection returns neighbor
  /// @param nthread
  /// @return
  py::tuple tree_data_unique_inverse(const DistT radius,
                                     const bool return_intersection,
                                     const int nthread) {

    using PairType = nanoflann::ResultItem<IndexType, DistT>;

    // in - self tree data
    const DataT* q_buf_ptr = tree_data_ptr_;
    const IndexType qlen = datalen_;

    // we don't need distance based sorting
    nanoflann::SearchParameters params;
    params.sorted = false;

    // out
    IndexVectorVector intersection{};
    if (return_intersection) {
      intersection.resize(qlen);
    }
    // prepare original inverse
    py::array_t<IndexType> original_inverse(qlen);
    IndexType* o_i_ptr =
        static_cast<IndexType*>(original_inverse.request().ptr);

    const IndexType index_t_dim{static_cast<IndexType>(dim_)};
    auto searchradius = [&](int start, int end, int current_tid) {
      for (IndexType i{static_cast<IndexType>(start)};
           i < static_cast<IndexType>(end);
           ++i) {

        // prepare input
        std::vector<PairType> matches;

        // call
        const auto nmatches = tree_->radiusSearch(&q_buf_ptr[i * index_t_dim],
                                                  radius,
                                                  matches,
                                                  params);

        // prepare output
        // set inverse_id
        IndexType unique_id;
        if (return_intersection) {
          auto& this_intersection = intersection[i];
          this_intersection.reserve(nmatches);
          for (auto& match : matches) {
            this_intersection.emplace_back(match.first);
          }
          std::sort(this_intersection.begin(), this_intersection.end());
          // set inverse_ids - it is the smallest neighbor (intersection) index
          unique_id = this_intersection[0];
        } else {
          // here, we'd only need min.
          const auto& min_match =
              *std::min_element(matches.begin(),
                                matches.end(),
                                [](const PairType& a, const PairType& b) {
                                  return a.first < b.first;
                                });
          unique_id = min_match.first;
        }
        o_i_ptr[i] = unique_id;
      }
    };

    nthread_execution(searchradius, static_cast<int>(qlen), nthread);

    return py::make_tuple<py::return_value_policy::move>(original_inverse,
                                                         intersection);
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

    auto searchradius = [&](int start, int end, int) {
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

    return py::make_tuple<py::return_value_policy::move>(out_indices, out_dist);
  }
};

template<typename T, unsigned int metric>
void add_kdt_pyclass(py::module_& m, const char* class_name) {
  using KDT = PyKDT<T, metric>;

  py::class_<KDT> klasse(m, class_name);

  klasse.def(py::init<>())
      .def(py::init<py::array_t<T>, size_t, int>(),
           py::arg("tree_data"),
           py::arg("leaf_size") = 10,
           py::arg("nthread") = 1)
      .def_readonly("tree_data", &KDT::tree_data_)
      .def_readonly("dim", &KDT::dim_)
      .def_readonly("metric", &KDT::metric_)
      .def("newtree",
           &KDT::newtree,
           py::arg("tree_data"),
           py::arg("leaf_size") = 10,
           py::arg("nthread") = 1)
      .def("knn_search",
           &KDT::knn_search,
           py::arg("queries"),
           py::arg("kneighbors"),
           py::arg("nthread"),
           py::return_value_policy::move)
      .def("query",
           &KDT::query,
           py::arg("queries"),
           py::arg("nthread"),
           py::return_value_policy::move)
      .def("radius_search",
           &KDT::radius_search,
           py::arg("queries"),
           py::arg("radius"),
           py::arg("return_sorted"),
           py::arg("nthread"),
           py::return_value_policy::move)
      .def("query_ball_point",
           &KDT::query_ball_point,
           py::arg("queries"),
           py::arg("radius"),
           py::arg("return_sorted"),
           py::arg("nthread"),
           py::return_value_policy::move)
      .def("radii_search",
           &KDT::radii_search,
           py::arg("queries"),
           py::arg("radii"),
           py::arg("return_sorted"),
           py::arg("nthread"),
           py::return_value_policy::move)
      .def("tree_data_unique_inverse",
           &KDT::tree_data_unique_inverse,
           py::arg("radius"),
           py::arg("return_intersection") = true,
           py::arg("nthread") = 1);
}

} // namespace napf
