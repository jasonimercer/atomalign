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
#include <dlib/matrix/matrix_math_functions.h>
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
  return dlib::sum(dlib::pointwise_multiply(M, M));
}

bool computeMean(const std::vector<Ptr>& src, Ptr u)
{
  double n = 0;

  if (src.empty())
  {
    return false;
  }

  (*u) = 0.0 * (*src[0]);

  for (size_t i = 0; i < src.size(); i++)
  {
    *u += *src[i];
    n++;
  }

  (*u) /= n;
  return true;
}

static double covE(const std::vector<Ptr>& src, Matrix::Ptr mean, int i, int j)
{
  double n = 0;
  double e = 0;

  if (src.empty())
  {
    return 0.0;
  }

  for (size_t k = 0; k < src.size(); k++)
  {
    e += ((*src[k])(i, 0) - (*mean)(i, 0)) *
         ((*src[k])(j, 0) - (*mean)(j, 0));
    n++;
  }

  if (n > 1)
  {
    n--;
  }

  return e / n;
}

bool computeCovariance(const std::vector<Ptr>& src, Ptr S)
{
  Matrix::Ptr mean(new Matrix::Type(1, 1));

  if (!S || !computeMean(src, mean))
  {
    return false;
  }

  const int n = mean->nr();

  S->set_size(n, n);
  (*S) *= 0.0;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      (*S)(i, j) = covE(src, mean, i, j);
    }
  }

  return true;
}

Ptr copy(Ptr A)
{
  if (!A)
  {
    return Matrix::Ptr();
  }

  Matrix::Ptr B(new Matrix::Type(*A));
  return B;
}

}  // namespace Matrix
