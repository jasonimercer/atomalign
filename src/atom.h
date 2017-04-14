/**
 * Software License Agreement CC0
 *
 * \file      atom.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef ATOM_H
#define ATOM_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "matrix.h"

class Atom
{
public:
  typedef boost::shared_ptr<Atom> Ptr;
  typedef double PosType;

  /**
   * @brief Default Atom Cosntructor
   */
  Atom();

  /**
   * @brief Copy constructor
   * @param a atom to copy
   */
  Atom(const Atom& a);

  /**
   * @brief get a copy of the Atom
   * @return smart pointer to a copy of the atom
   */
  Atom::Ptr copy() const;

  /**
   * @brief Custom atom name (example: H1, C4b)
   */
  std::string name_;

  /**
   * @brief Atom type, see AtomProperties
   */
  std::string type_;

  /**
   * @brief Atom position
   */
  Matrix::Ptr pos_;  // column matrix expected
};

#endif // ATOM_H
