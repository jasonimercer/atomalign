#include "atomcontainer.h"
#include "matrix.h"
#include <dlib/optimization.h>
#include <math.h>
#include <algorithm>
#include <boost/function.hpp>
#include <boost/bind.hpp>

AtomContainer::AtomContainer()
{
}

AtomContainer::~AtomContainer()
{
}

void AtomContainer::extend(AtomContainer::Ptr ac)
{
  if (ac)
  {
    extend(*ac);
  }
}

void AtomContainer::extend(const AtomContainer& ac)
{
  for (size_t i = 0; i < ac.atoms_.size(); i++)
  {
    atoms_.push_back(Atom::Ptr(new Atom(*ac.atoms_[i])));
  }
}

void AtomContainer::transform(double dx, double dy, double dz,
                              double rx, double ry, double rz)
{
  Matrix::Type Rot, RX, RY, RZ;
  Matrix::makeRotationX(RX, rx);
  Matrix::makeRotationY(RY, ry);
  Matrix::makeRotationZ(RZ, rz);

  Rot = (RX * RY) * RZ;

  Matrix::Type d(3, 1);
  d(0, 0) = dx;
  d(1, 0) = dy;
  d(2, 0) = dz;

  for (size_t i = 0; i < atoms_.size(); i++)
  {
    (*atoms_[i]->pos_) = Rot * (*atoms_[i]->pos_) + d;
  }
}

void AtomContainer::untransform(double dx, double dy, double dz,
                                double rx, double ry, double rz)
{
  Matrix::Type invRot, RX, RY, RZ;
  Matrix::makeRotationX(RX, rx);
  Matrix::makeRotationY(RY, ry);
  Matrix::makeRotationZ(RZ, rz);

  invRot = dlib::inv((RX * RY) * RZ);

  Matrix::Type d(3, 1);
  d(0, 0) = dx;
  d(1, 0) = dy;
  d(2, 0) = dz;

  for (size_t i = 0; i < atoms_.size(); i++)
  {
    (*atoms_[i]->pos_) = invRot * ((*atoms_[i]->pos_) - d);
  }
}

AtomContainer::Ptr AtomContainer::copy()
{
  AtomContainer::Ptr p(new AtomContainer());
  p->extend(*this);
  return p;
}

bool AtomContainer::near(Matrix::Ptr p, size_t& idx, double& dist2) const
{
  if (atoms_.empty())
  {
    return false;
  }

  if (!p)
  {
    return false;
  }

  idx = 0;
  dist2 = Matrix::sumOfSquares(*p - *atoms_[0]->pos_);

  for (size_t i = 1; i < atoms_.size(); i++)
  {
    const double dist2_i = Matrix::sumOfSquares(*p - *atoms_[i]->pos_);

    if (dist2_i < dist2)
    {
      dist2 = dist2_i;
      idx = i;
    }
  }

  return true;
}

void AtomContainer::intersect(const AtomContainer& ac, double tol)
{
  size_t closest_idx;
  double closest_dist2;
  const double tol2 = tol * tol;

  std::vector<Atom::Ptr> good;

  for (size_t i = 0; i < atoms_.size(); i++)
  {
    if (ac.near(atoms_[i]->pos_, closest_idx, closest_dist2))
    {
      if (closest_dist2 <= tol2)
      {
        good.push_back(atoms_[i]);
      }
    }
  }

  // replace vector with atoms that are within tol of atoms in other container
  atoms_ = good;
}

double AtomContainer::closestDistanceSquared(const AtomContainer& a, const AtomContainer& b)
{
  double sum = 0;

  for (size_t i = 0; i < a.atoms_.size(); i++)
  {
    double closest = 1e40;
    for (size_t j = 0; j < b.atoms_.size(); j++)
    {
      const double dist2_ij = Matrix::sumOfSquares(*a.atoms_[i]->pos_ - *b.atoms_[j]->pos_);
      closest = std::min<double>(closest, dist2_ij);
    }
    sum += closest;
  }

  return sum;
}

typedef dlib::matrix<double,0,1> column_vector;

static double objective_function(AtomContainer::Ptr a,
                                 AtomContainer::Ptr b,
                                 const column_vector& X)
{
  AtomContainer::Ptr c = a->copy();

  c->transform(X(0), X(1), X(2), X(3), X(4), X(5));

  return
      // checking both ways:
      AtomContainer::closestDistanceSquared(*b, *c) +
      AtomContainer::closestDistanceSquared(*c, *b);
}

double AtomContainer::align(AtomContainer::Ptr a, AtomContainer::Ptr b,
                            double& dx, double& dy, double& dz,
                            double& rx, double& ry, double& rz,
                            double rho_begin, double rho_end, int steps)
{

  column_vector X(6);
  X(0) = dx;
  X(1) = dy;
  X(2) = dz;
  X(3) = rx;
  X(4) = ry;
  X(5) = rz;

  const int n = 6;
  const int i = (n + 1) * (n + 2) / 2;

  boost::function<double (const column_vector&)> f = boost::bind(objective_function, a, b, _1);

  try
  {
    dlib::find_min_bobyqa(f,
                          X,
                          i,    // number of interpolation points
                          dlib::uniform_matrix<double>(6, 1, -1e10),  // lower bound constraint
                          dlib::uniform_matrix<double>(6, 1, 1e10),   // upper bound constraint
                          rho_begin,  // initial trust region radius
                          rho_end,  // stopping trust region radius
                          steps  // max number of objective function evaluations
                          );
  }
  catch(dlib::bobyqa_failure ex)
  {
    fprintf(stderr, "%s\n", ex.what());
    // find_min_bobyqa will throw is it fails to converge
  }

  dx = X(0);
  dy = X(1);
  dz = X(2);
  rx = X(3);
  ry = X(4);
  rz = X(5);
  return f(X);
}
