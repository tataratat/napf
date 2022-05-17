#pragma once

#include <nanoflann.hpp>


namespace napf {
/*
 * 
 *
 *
 *
 */
template <typename PointT, size_t dim>
struct RawPtrCloud {
public:
  RawPtrCloud(const PointT* points, size_t ptrlen) :
      points_(points) {}

  inline sizt_t kdtree_get_point_count() const {
    return ptrlen_ / dim_;
  }

  inline PointT kdtree_get_pt(const size_t q_ind,  const size_t q_dim) const {
    return points_[q_ind * dim + q_dim];
  }

  template <class BBOX>
  bool kdtree_get_bbox(BBOX&) const {return false;}

private:
  const PointT* points_;
  const size_t ptrlen_;
  const size_t dim_ = dim;
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
template<typename T, size_t dim, unsigned int metric>
using RawPtrTree = nanoflann::KDTreeSingleIndexAdaptor<
    std::conditional_t<
      (metric == 1),
      nanoflann::L1_Adaptor<T, RawPtrCloud<T, dim>>,
      nanoflann::L2_Simple_Adaptor<T, RawPtrCloud<T, dim>>
    >,
    RawPtrCloud<T, dim>,
    dim>;

template<typename T, size_t dim, unsigned int metric>
using RawPtrHighDimTree = nanoflann::KDTreeSingleIndexAdaptor<
    std::conditional_t<
      (metric == 1),
      nanoflann::L1_Adaptor<T, RawPtrCloud<T, dim>>,
      nanoflann::L2_Adaptor<T, RawPtrCloud<T, dim>>
    >,
    RawPtrCloud<T, dim>,
    dim>;


#ifdef SPLINELIB
template<typename VSCoords /* std::vector<VectorSpace::Coordinate>*/>
struct VSCoordCloud {
public:
  const VSCoords& points_;

  VSCoordCloud(const VSCoords& vscoords) :
      points_(points) {}

  // CRTP helper method
  inline const VSCoords& pts() const {return points_;}

  inline size_t kdtree_get_point_count() const {
    return pts().size();
  }

  // Since the type is hardcoded in splinelib, here too.
  inline double& kdtree_get_pt(const size_t id, const size_t dim) const {
    return pts()[id][dim];
  }

  // everyone does it
  template <class BBOX>
  bool kdtree_get_bbox(BBOX&) const {return false;}

};

template<size_t dim, unsigned int metric, typename VSCCloud>
using SplineLibCoordinatesTree = nanoflann::KDTreeSingleIndexAdaptor<
    std::conditional_t<
      (metric == 1),
      nanoflann::L1_Adaptor<T, VSCCloud>,
      nanoflann::L2_Simple_Adaptor<T, VSCCloud>
    >,
    VSCCloud,
    dim>;

template<size_t dim, unsigned int metric, typename VSCCloud>
using SplinelibCoordinatesHighDimTree = nanoflann::KDTreeSingleIndexAdaptor<
    std::conditional_t<
      (metric == 1),
      nanoflann::L1_Adaptor<double, VSCCloud>,
      nanoflann::L2_Adaptor<double, VSCCloud>
    >,
    VSCCloud,
    dim>;

/* Incase you don't have time to write the `SplineLib` */
template<size_t dim, unsigned int metric, typename VSCCloud>
using SLCoordTree = SplineLibCoordinatesTree<dim, metric, VSCCloud>;

template<size_t dim, unsigned int metric, typename VSCCloud>
using SLCoordHighDimTree = SplineLibCoordinatesHighDimTree<
    dim,
    metric,
    VSCCloud>;

#endif 

}; /* namespace */
