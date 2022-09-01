#pragma once

#include <nanoflann.hpp>

namespace napf {

template<typename PointT, typename IndexT, int dim>
struct RawPtrCloud {
public:
  RawPtrCloud() = default;

  RawPtrCloud(const PointT* points, IndexT ptrlen)
      : points_(points),
        ptrlen_(ptrlen) {}

  inline size_t kdtree_get_point_count() const { return ptrlen_ / dim_; }

  inline PointT kdtree_get_pt(const IndexT q_ind, const IndexT q_dim) const {
    return points_[q_ind * dim + q_dim];
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
using RawPtrTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::
            L1_Adaptor<DataT, RawPtrCloud<DataT, IndexT, dim>, DistT, IndexT>,
        nanoflann::
            L2_Adaptor<DataT, RawPtrCloud<DataT, IndexT, dim>, DistT, IndexT>>::
        type,
    RawPtrCloud<DataT, IndexT, dim>,
    dim,
    IndexT>;

template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric>
using RawPtrHighDimTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::
            L1_Adaptor<DataT, RawPtrCloud<DataT, IndexT, dim>, DistT, IndexT>,
        nanoflann::
            L2_Adaptor<DataT, RawPtrCloud<DataT, IndexT, dim>, DistT, IndexT>>::
        type,
    RawPtrCloud<DataT, IndexT, dim>,
    dim,
    IndexT>;

#ifdef SPLINEPYEXT
// CoordinatesT : std::unique_ptr<
//                        std::vector<std::array<CoordinateValueT, dim>>
//                >
template<typename CoordinatesT, typename IndexT, int dim>
struct CoordinatesCloud {
public:
  using CoordnatesT_ = CoordinatesT;
  using IndexT_ = IndexT;

  const CoordinatesT& points_;
  const IndexT size_;

  CoordinatesCloud(const CoordinatesT& coords, const IndexT size)
      : points_(coords),
        size_(size) {}

  // CRTP helper method
  inline const CoordinatesT& pts() const { return points_; }

  inline size_t kdtree_get_point_count() const { return size_; }

  // Since the type is hardcoded in splinelib, here too.
  inline double kdtree_get_pt(const IndexT id, const IndexT q_dim) const {
    // cast here to allow both SplineLib and BezMan coordinates types.
    return static_cast<double>(pts()[id][q_dim]);
  }

  // everyone does it
  template<class BBOX>
  bool kdtree_get_bbox(BBOX&) const {
    return false;
  }
};

template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric,
         typename CoordinatesCloudT>
using CoordinatesTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::L1_Adaptor<DataT, CoordinatesCloudT, DistT, IndexT>,
        nanoflann::L2_Simple_Adaptor<DataT, CoordinatesCloudT, DistT, IndexT>>::
        type,
    CoordinatesCloudT,
    dim,
    IndexT>;

template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric,
         typename CoordinatesCloudT>
using CoordinatesHighDimTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::L1_Adaptor<DataT, CoordinatesCloudT, DistT, IndexT>,
        nanoflann::L2_Adaptor<DataT, CoordinatesCloudT, DistT, IndexT>>::type,
    CoordinatesCloudT,
    dim,
    IndexT>;
#endif

} // namespace napf
