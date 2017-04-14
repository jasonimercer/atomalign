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
}  // namespace Matrix

#endif  // MATRIX_H
