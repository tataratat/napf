#pragma once

#include <nanoflann.hpp>

namespace napf {

template<typename DataT, typename IndexT>
struct ArrayCloud {
public:
  ArrayCloud() = default;

  ArrayCloud(const DataT* points, IndexT ptrlen, IndexT dim)
      : points_(points),
        ptrlen_(ptrlen),
        dim_(dim) {}

  inline size_t kdtree_get_point_count() const { return ptrlen_ / dim_; }

  inline const DataT& kdtree_get_pt(const IndexT& q_ind,
                                    const IndexT& q_dim) const {
    return points_[q_ind * dim_ + q_dim];
  }

  template<class BBOX>
  bool kdtree_get_bbox(BBOX&) const {
    return false;
  }

private:
  const DataT* points_;
  const IndexT ptrlen_;
  const IndexT dim_;
};

/*
 * KDTree based on RawPtrs
 *
 * TParameters
 * ------------
 * T: data type
 * metric: distance matric
 *  1 -> L1, 2 -> L2
 */
template<typename DataT, typename DistT, typename IndexT, unsigned int metric>
using ArrayTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::L1_Adaptor<DataT, ArrayCloud<DataT, IndexT>, DistT, IndexT>,
        nanoflann::L2_Simple_Adaptor<DataT,
                                     ArrayCloud<DataT, IndexT>,
                                     DistT,
                                     IndexT>>::type,
    ArrayCloud<DataT, IndexT>,
    -1, /* dim is dynamic variable in nanoflann, so let's not extent tparam */
    IndexT>;

/// helper type for Distance. It will be a double unless DataT is float.
template<typename DataT>
using DistT = typename std::
    conditional<std::is_same<DataT, float>::value, float, double>::type;

} // namespace napf
