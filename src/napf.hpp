#pragma once

#include <nanoflann.hpp>


namespace napf {
/*
 * 
 *
 *
 *
 */
template <typename PointT, typename IndexT, int dim>
struct RawPtrCloud {
public:

  RawPtrCloud() = default;

  RawPtrCloud(const PointT* points, IndexT ptrlen) :
      points_(points), ptrlen_(ptrlen) {}

  inline size_t kdtree_get_point_count() const {
    return ptrlen_ / dim_;
  }

  inline PointT kdtree_get_pt(const IndexT q_ind,  const IndexT q_dim) const {
    return points_[q_ind * dim + q_dim];
  }

  template <class BBOX>
  bool kdtree_get_bbox(BBOX&) const {return false;}

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
template<typename datat,
         typename distt,
         typename indext,
         int dim,
         unsigned int metric>
using rawptrtree = nanoflann::kdtreesingleindexadaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::l1_adaptor<datat,
                              rawptrcloud<datat, indext, dim>,
                              distt,
                              indext>,
        nanoflann::l2_simple_adaptor<datat,
                                     rawptrcloud<datat, indext, dim>,
                                     distt,
                                     indext>
    >::type,
    rawptrcloud<datat, indext, dim>,
    dim,
    indext>;

template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric>
using RawPtrHighDimTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::L1_Adaptor<DataT,
                              RawPtrCloud<DataT, IndexT, dim>,
                              DistT,
                              IndexT>,
        nanoflann::L2_Adaptor<DataT,
                              RawPtrCloud<DataT, IndexT, dim>,
                              DistT,
                              IndexT>
    >::type,
    RawPtrCloud<DataT, IndexT, dim>,
    dim,
    IndexT>;


#ifdef SPLINELIBEXT
template<typename VSCoords /* std::vector<VectorSpace::Coordinate>*/,
         typename IndexT,
         int dim>
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
  inline double& kdtree_get_pt(const IndexT id, const IndexT dim) const {
    return pts()[id][dim];
  }

  // everyone does it
  template <class BBOX>
  bool kdtree_get_bbox(BBOX&) const {return false;}

};

template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric,
         typename VSCCloud>
using SplineLibCoordinatesTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::L1_Adaptor<DataT,
                              VSCCloud,
                              DistT,
                              IndexT>,
        nanoflann::L2_Simple_Adaptor<DataT,
                                     VSCCloud,
                                     DistT,
                                     IndexT>
    >::type,
    VSCCloud,
    dim,
    IndexT>;


template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric,
         typename VSCCloud>
using SplinelibCoordinatesHighDimTree = nanoflann::KDTreeSingleIndexAdaptor<
    typename std::conditional<
        (metric == 1),
        nanoflann::L1_Adaptor<DataT,
                              VSCCloud,
                              DistT,
                              IndexT>,
        nanoflann::L2_Adaptor<DataT,
                              VSCCloud,
                              DistT,
                              IndexT>
    >::type,
    VSCCloud,
    dim,
    IndexT>;

/* Incase you don't have time to write the `SplineLib` */
template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric,
         typename VSCCloud>
using SLCTree = SplineLibCoordinatesTree<DataT,
                                         DistT,
                                         IndexT,
                                         dim,
                                         metric,
                                         VSCCloud>;

template<typename DataT,
         typename DistT,
         typename IndexT,
         size_t dim,
         unsigned int metric,
         typename VSCCloud>
using SLCHDTree = SplineLibCoordinatesHighDimTree<DataT,
                                                  DistT,
                                                  IndexT,
                                                  dim,
                                                  metric,
                                                  VSCCloud>;
#endif 

}; /* namespace */
