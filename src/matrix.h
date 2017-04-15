/**
 * Software License Agreement CC0
 *
 * \file      matrix.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <boost/shared_ptr.hpp>
#include <dlib/matrix.h>
#include <vector>

#ifndef MATRIX_H
#define MATRIX_H

namespace Matrix
{
  typedef dlib::matrix<double, 0, 0> Type;
  typedef boost::shared_ptr<Type> Ptr;

  void makeRotationX(Type& R, double theta);
  void makeRotationY(Type& R, double theta);
  void makeRotationZ(Type& R, double theta);

  double sumOfSquares(const Type& M);

  /**
   * @brief Compute the mean of a source vector
   * @param src vector
   * @param u mean
   * @return true if mean was computed
   */
  bool computeMean(const std::vector<Ptr>& src, Ptr u);

  /**
   * @brief Compute the covariance of the source vector
   * @param src vector
   * @param u mean
   * @return true if covariance was computed
   */
  bool computeCovariance(const std::vector<Ptr>& src, Ptr S);

  /**
   * @brief Create a pointer to a copy of the given matrix
   * @param A src matrix
   * @return copy of matrix
   */
  Ptr copy(Ptr A);
}  // namespace Matrix

#endif  // MATRIX_H
