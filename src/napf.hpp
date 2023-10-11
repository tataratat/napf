#pragma once

#include <nanoflann.hpp>

namespace napf {

template<typename PointT, typename IndexT, int dim>
struct ArrayCloud {
public:
  ArrayCloud() = default;

  ArrayCloud(const PointT* points, IndexT ptrlen)
      : points_(points),
        ptrlen_(ptrlen) {}

  inline size_t kdtree_get_point_count() const { return ptrlen_ / dim_; }

  inline PointT kdtree_get_pt(const IndexT& q_ind, const IndexT& q_dim) const {
    return points_[q_ind * dim_ + q_dim];
  }

  template<class BBOX>
  bool kdtree_get_bbox(BBOX&) const {
    return false;
  }

private:
  const PointT* points_;
  const IndexT ptrlen_;
  const IndexT dim_ = static_cast<IndexT>(dim);
};

/*
 * KDTree based on RawPtrs
 *
 * TParameters
 * ------------
 * T: data type
 * dim: data dim
 * metric: distance matric
 *  1 -> L1, 2 -> L2
 */
template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric>
using ArrayTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::
            L1_Adaptor<DataT, ArrayCloud<DataT, IndexT, dim>, DistT, IndexT>,
        typename std::conditional<
            (dim > 3),
            nanoflann::L2_Simple_Adaptor<DataT,
                                         ArrayCloud<DataT, IndexT, dim>,
                                         DistT,
                                         IndexT>,
            nanoflann::L2_Adaptor<DataT,
                                  ArrayCloud<DataT, IndexT, dim>,
                                  DistT,
                                  IndexT>>::type>::type,
    ArrayCloud<DataT, IndexT, dim>,
    dim,
    IndexT>;
} // namespace napf
