/**
 * Software License Agreement CC0
 *
 * \file      atom.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "atom.h"

Atom::Atom()
{
  pos_ = Matrix::Ptr(new Matrix::Type(3, 1));
}

Atom::Atom(const Atom& a)
{
  pos_ = Matrix::Ptr(new Matrix::Type(3, 1));

  (*pos_)(0, 0) = (*a.pos_)(0, 0);
  (*pos_)(1, 0) = (*a.pos_)(1, 0);
  (*pos_)(2, 0) = (*a.pos_)(2, 0);

  type_ = a.type_;
  name_ = a.name_;
}

Atom::Ptr Atom::copy() const
{
  return Atom::Ptr(new Atom(*this));
}
