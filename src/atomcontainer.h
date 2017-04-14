#ifndef ATOMCONTAINER_H
#define ATOMCONTAINER_H

#include <boost/shared_ptr.hpp>
#include "atom.h"
#include <string>
#include <vector>


class AtomContainer
{
public:
  typedef boost::shared_ptr<AtomContainer> Ptr;

  AtomContainer();
  ~AtomContainer();

  std::vector<Atom::Ptr> atoms_;

  /**
   * @brief Add a copy of the provided atoms to the current container
   * @param ac Atom container with new atoms to be copied
   */
  void extend(AtomContainer::Ptr ac);

  /**
   * @brief Add a copy of the provided atoms to the current container
   * @param ac Atom container with new atoms to be copied
   */
  void extend(const AtomContainer& ac);

  /**
   * @brief Transform a container by applying translations and rotations
   * @param[in] dx,dy,dz Displacement values
   * @param[in] rx,ry,rz Rotation values
   */
  void transform(double dx, double dy, double dz,
                 double rx, double ry, double rz);

  /**
   * @brief Untransform a container by applying the inverse operations from a transform
   * @param[in] dx,dy,dz Displacement values
   * @param[in] rx,ry,rz Rotation values
   */
  void untransform(double dx, double dy, double dz,
                   double rx, double ry, double rz);

  /**
   * @brief Make a copy of this container
   * @return copy
   */
  AtomContainer::Ptr copy();

  /**
   * @brief Get the index of the atom nearest to the provided point
   * @param[in] p Point
   * @param[out] idx Closest index
   * @param[out] dist2 Distance squared between given atom and nearest atom
   * @return true if container has atoms
   */
  bool near(Matrix::Ptr p, size_t& idx, double& dist2) const;

  /**
   * @brief Intersect the calling conatainer against another. Atoms in the calling
   *        container that are greater than tol distance away from an atom in the
   *        other container will be removed from the container
   * @param ac Pattern container
   * @param tol Maximum distance on mismatched atoms allowed
   */
  void intersect(const AtomContainer& ac, double tol);

  /**
   * @brief Compute the sum of the squares of the closest distances between each
   *        atom in a and the closest in b
   * @param a Container A
   * @param b Container B
   * @return sum of squares of closest distances
   */
  static double closestDistanceSquared(const AtomContainer& a,
                                       const AtomContainer& b);

  /**
   * @brief Attempt to align two atom containers
   * @param[in] a Atom container "a"
   * @param[in] b Atom container "b"
   * @param[in,out] dx,dy,dz Displacement values
   * @param[in,out] rx,ry,rz Rotation values
   * @param[in] rho_begin initial tolerance
   * @param[in] rho_begin final tolerance
   * @param[in] steps Number of steps
   * @return distance squared between final positions
   */
  static double align(AtomContainer::Ptr a, AtomContainer::Ptr b,
                      double& dx, double& dy, double& dz,
                      double& rx, double& ry, double& rz,
                      double rho_begin, double rho_end,
                      int steps);
};

#endif // ATOMCONTAINER_H
