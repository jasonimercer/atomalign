/**
 * Software License Agreement CC0
 *
 * \file      matrix.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "matrix.h"
#include <math.h>

namespace Matrix
{

void makeRotationX(Type& R, double theta)
{
  double c, s;
  sincos(theta, &s, &c);

  R = dlib::identity_matrix<double,long>(3);
  R(1, 1) = c;
  R(1, 2) = -s;
  R(2, 1) = s;
  R(2, 2) = c;
}

void makeRotationY(Type& R, double theta)
{
  double c, s;
  sincos(theta, &s, &c);

  R = dlib::identity_matrix<double,long>(3);
  R(0, 0) = c;
  R(0, 2) = -s;
  R(2, 0) = s;
  R(2, 2) = c;
}

void makeRotationZ(Type& R, double theta)
{
  double c, s;
  sincos(theta, &s, &c);

  R = dlib::identity_matrix<double,long>(3);
  R(0, 0) = c;
  R(0, 1) = -s;
  R(1, 0) = s;
  R(1, 1) = c;
}

double sumOfSquares(const Type& M)
{
  double sum = 0;

  for (int i = 0; i < M.nr(); i++)
  {
    for (int j = 0; j < M.nc(); j++)
    {
      sum += M(i, j) * M(i, j);
    }
  }

  return sum;
}

}  // namespace Matrix
