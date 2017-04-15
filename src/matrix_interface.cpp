/**
 * Software License Agreement CC0
 *
 * \file      matrix_interface.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <dlib/matrix/matrix_utilities.h>
#include "matrix_interface.h"
#include <stdio.h>

using namespace LuaInterface;

std::string MatrixInterface::typeName()
{
  return "Matrix";
}

uint32_t MatrixInterface::hash()
{
  return COMPILE_TIME_CRC32_STR("Matrix");
}

Matrix::Ptr MatrixInterface::as(lua_State* L, int idx)
{
  Matrix::Ptr m(new Matrix::Type);

  idx = lua_absindex(L, idx);

  if (luaT_is<Matrix::Type>(L, idx))
  {
    *m = *luaT_to<Matrix::Type>(L, idx);
  }

  if (lua_istable(L, idx))
  {
    std::vector< std::vector<double> > v;

    lua_pushnil(L);

    while (lua_next(L, idx))
    {
      v.push_back(std::vector<double>());

      int tab_pos = lua_gettop(L);
      if (lua_istable(L, tab_pos))
      {
        lua_pushnil(L);

        while (lua_next(L, tab_pos))
        {
          v.back().push_back(lua_tonumber(L, -1));
          lua_pop(L, 1);
        }
      }
      else
      {
        if (lua_isnumber(L, tab_pos))
        {
          v.back().push_back(lua_tonumber(L, tab_pos));
        }
      }

      lua_pop(L, 1);
    }

    if (v.size())
    {
      size_t cols = v[0].size();
      size_t rows = v.size();

      for (size_t r = 0; r < rows; r++)
      {
        if (v[r].size() != v[0].size())
        {
          luaL_error(L, "unequal columns detected: rows %i and %i", r + 1, 1);
          return Matrix::Ptr();
        }
      }

      m->set_size(rows, cols);

      for (size_t i = 0; i < rows; i++)
      {
        for (size_t j = 0; j < cols; j++)
        {
          (*m)(i, j) = v[i][j];
        }
      }
    }
    else
    {
      m->set_size(0, 0);
    }
  }

  return m;
}


int MatrixInterface::l_new(lua_State* L)
{
  Matrix::Ptr m = MatrixInterface::as(L, 1);

  return luaT_push(L, m);
}

static bool sameSize(Matrix::Ptr a, Matrix::Ptr b)
{
  return a->nc() == b->nc() && a->nr() == b->nr();
}

static int l_add(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);
  Matrix::Ptr B = luaT_to<Matrix::Type>(L, 2);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  if (!B)
  {
    return luaL_argerror(L, 2, "Matrix Expected");
  }

  if (!sameSize(A, B))
  {
    return luaL_error(L, "size mismatch");
  }

  Matrix::Ptr A_B(new Matrix::Type);

  (*A_B) = (*A) + (*B);

  luaT_push(L, A_B);
  return 1;
}

static int l_sub(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);
  Matrix::Ptr B = luaT_to<Matrix::Type>(L, 2);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  if (!B)
  {
    return luaL_argerror(L, 2, "Matrix Expected");
  }

  if (!sameSize(A, B))
  {
    return luaL_error(L, "size mismatch");
  }

  Matrix::Ptr A_B(new Matrix::Type);

  (*A_B) = (*A) - (*B);

  luaT_push(L, A_B);
  return 1;
}

static int l_trans(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  Matrix::Ptr B(new Matrix::Type);

  *B = dlib::trans(*A);

  luaT_push(L, B);
  return 1;
}

static int l_inv(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  Matrix::Ptr B(new Matrix::Type);

  *B = dlib::inv(*A);

  luaT_push(L, B);
  return 1;
}

static int l_det(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  lua_pushnumber(L, dlib::det(*A));
  return 1;
}

static int l_unm(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);
  Matrix::Ptr B(new Matrix::Type);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  *B = (*A) * -1.0;

  luaT_push(L, B);
  return 1;
}

static int l_reshape(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  int new_rows = lua_tointeger(L, 2);
  int new_cols = lua_tointeger(L, 3);

  *A = dlib::reshape(*A, new_rows, new_cols);

  return 0;
}

static int l_reshaped(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  luaT_push(L, Matrix::copy(A));
  lua_replace(L, 1);
  l_reshape(L);

  lua_settop(L, 1);  // remove all but the reshaped copy

  return 1;
}


static int l_round(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  *A = dlib::round(*A);

  return 0;
}

static int l_rounded(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  luaT_push(L, Matrix::copy(A));
  lua_replace(L, 1);
  l_round(L);

  lua_settop(L, 1);  // remove all but the rounded copy

  return 1;
}

static int l_svd(lua_State* L)
{
  Matrix::Ptr M = luaT_to<Matrix::Type>(L, 1);

  if (!M)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  const int nr = M->nr();
  const int nc = M->nc();

  Matrix::Ptr U(new Matrix::Type);
  Matrix::Ptr S(new Matrix::Type);
  Matrix::Ptr VT(new Matrix::Type);

  dlib::svd(*M, *U, *S, *VT);

  /*
  - computes the singular value decomposition of m
  - M == U * S * V^T
  - U^T * U == identity matrix
  - V^T * V == identity matrix
  - diag(W) == the singular values of the matrix m in no
    particular order.  All non-diagonal elements of S are
    set to 0.
  - U.nr() == M.nr()
  - U.nc() == M.nc()
  - S.nr() == M.nc()
  - S.nc() == M.nc()
  - V.nr() == M.nc()
  - V.nc() == M.nc()
  */

  luaT_push(L, U);
  luaT_push(L, S);
  luaT_push(L, VT);
  return 3;
}

static int l_mul(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  if (luaT_is<Matrix::Type>(L, 2))
  {
    Matrix::Ptr B = luaT_to<Matrix::Type>(L, 2);

    if (A->nc() != B->nr())
    {
      return luaL_error(L, "Size mismatch");
    }

    Matrix::Ptr AB(new Matrix::Type);

    *AB = (*A) * (*B);

    luaT_push(L, AB);
    return 1;
  }

  if (lua_isnumber(L, 2))
  {
    double n = lua_tonumber(L, 2);
    Matrix::Ptr nA(new Matrix::Type(*A));

    (*nA) = (*A) * n;

    luaT_push(L, nA);
    return 1;
  }

  return 0;
}

static int l_div(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  if (luaT_is<Matrix::Type>(L, 2))
  {
    Matrix::Ptr B = luaT_to<Matrix::Type>(L, 2);

    if (B->nc() != A->nr())
    {
      return luaL_error(L, "Size mismatch");
    }

    Matrix::Ptr AB(new Matrix::Type);

    *AB = dlib::inv(*B) * (*A);

    luaT_push(L, AB);
    return 1;
  }

  if (lua_isnumber(L, 2))
  {
    double n = lua_tonumber(L, 2);
    Matrix::Ptr nA(new Matrix::Type(*A));

    if (n == 0)
    {
      (*nA) = (*A) * n;
    }
    else
    {
      (*nA) = (*A) / n;
    }

    luaT_push(L, nA);
    return 1;
  }

  return 0;
}

static int l_tostring(lua_State* L)
{
  Matrix::Ptr m = luaT_to<Matrix::Type>(L, 1);

  if (!m)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  lua_getglobal(L, "tostring");

  std::string s = "Matrix({";

  for (int r = 0; r < m->nr(); r++)
  {
    if (r)
    {
      s += ", ";
    }

    s += "{";
    for (int c = 0; c < m->nc(); c++)
    {
      if (c)
      {
        s += ", ";
      }

      lua_pushvalue(L, -1);
      lua_pushnumber(L, (*m)(r, c));
      lua_call(L, 1, 1);

      s += lua_tostring(L, -1);
      lua_pop(L, 1);
    }
    s += "}";
  }
  s += "})";

  lua_pushstring(L, s.c_str());
  return 1;
}

static int l_set(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  int r = lua_tointeger(L, 2) - 1;
  int c = lua_tointeger(L, 3) - 1;
  double x = lua_tonumber(L, 4);

  if (r < 0 || r >= A->nr())
  {
    return luaL_error(L, "row out of range");
  }
  if (c < 0 || c >= A->nc())
  {
    return luaL_error(L, "col out of range");
  }

  (*A)(r, c) = x;

  return 0;
}

static int l_get(lua_State* L)
{
  Matrix::Ptr A = luaT_to<Matrix::Type>(L, 1);

  if (!A)
  {
    return luaL_argerror(L, 1, "Matrix Expected");
  }

  int r = lua_tointeger(L, 2) - 1;
  int c = 0;

  if (lua_isnumber(L, 3))
  {
    c = lua_tointeger(L, 3) - 1;
  }

  if (r < 0 || r >= A->nr())
  {
    return luaL_error(L, "row out of range");
  }
  if (c < 0 || c >= A->nc())
  {
    return luaL_error(L, "col out of range");
  }

  lua_pushnumber(L, (*A)(r, c));

  return 1;
}

std::vector<luaL_Reg> MatrixInterface::luaMethods()
{
  std::vector<luaL_Reg> methods;
  methods.push_back(luaL_toreg("tr", l_trans));
  methods.push_back(luaL_toreg("inv", l_inv));
  methods.push_back(luaL_toreg("det", l_det));
  methods.push_back(luaL_toreg("svd", l_svd));
  methods.push_back(luaL_toreg("reshape", l_reshape));
  methods.push_back(luaL_toreg("reshaped", l_reshaped));

  methods.push_back(luaL_toreg("round", l_round));
  methods.push_back(luaL_toreg("rounded", l_rounded));

  methods.push_back(luaL_toreg("__tostring", l_tostring));
  methods.push_back(luaL_toreg("__mul", l_mul));
  methods.push_back(luaL_toreg("__div", l_div));
  methods.push_back(luaL_toreg("__add", l_add));
  methods.push_back(luaL_toreg("__sub", l_sub));
  methods.push_back(luaL_toreg("__unm", l_unm));
  methods.push_back(luaL_toreg("set", l_set));
  methods.push_back(luaL_toreg("get", l_get));

  return methods;
}


static int l_makerotx(lua_State* L)
{
  double theta = lua_tonumber(L, 1);
  Matrix::Ptr m(new Matrix::Type);
  Matrix::makeRotationX(*m, theta);
  luaT_push(L, m);
  return 1;
}

static int l_makeroty(lua_State* L)
{
  double theta = lua_tonumber(L, 1);
  Matrix::Ptr m(new Matrix::Type);
  Matrix::makeRotationY(*m, theta);
  luaT_push(L, m);
  return 1;
}

static int l_makerotz(lua_State* L)
{
  double theta = lua_tonumber(L, 1);
  Matrix::Ptr m(new Matrix::Type);
  Matrix::makeRotationZ(*m, theta);
  luaT_push(L, m);
  return 1;
}

static int l_makeident(lua_State* L)
{
  int n = 3;
  if (lua_isinteger(L, 1))
  {
    n = lua_tointeger(L, 1);
  }
  if (n < 1)
  {
    n = 1;
  }
  Matrix::Ptr m(new Matrix::Type);
  *m = dlib::identity_matrix<double,long>(n);

  luaT_push(L, m);
  return 1;
}

static int l_mean(lua_State* L)
{
  std::vector<Matrix::Ptr> X;

  for (int i = 1; i <= lua_gettop(L); i++)
  {
    if (lua_istable(L, i))
    {
      lua_pushnil(L);

      while (lua_next(L, i))
      {
        Matrix::Ptr x = MatrixInterface::as(L, -1);

        if (x)
        {
          X.push_back(x);
        }
        lua_pop(L, 1);
      }
    }
  }

  Matrix::Ptr u(new Matrix::Type);

  if (Matrix::computeMean(X, u))
  {
    luaT_push<Matrix::Type>(L, u);
    return 1;
  }

  return 0;
}

static int l_cov(lua_State* L)
{
  std::vector<Matrix::Ptr> X;

  for (int i = 1; i <= lua_gettop(L); i++)
  {
    if (lua_istable(L, i))
    {
      lua_pushnil(L);

      while (lua_next(L, i))
      {
        Matrix::Ptr x = MatrixInterface::as(L, -1);

        if (x)
        {
          X.push_back(x);
        }
        lua_pop(L, 1);
      }
    }
  }

  Matrix::Ptr S(new Matrix::Type);

  if (Matrix::computeCovariance(X, S))
  {
    luaT_push<Matrix::Type>(L, S);
    return 1;
  }

  return 0;
}

std::vector<luaL_Reg> MatrixInterface::luaFunctions()
{
  std::vector<luaL_Reg> functions;
  functions.push_back(luaL_toreg("makeRotationX", l_makerotx));
  functions.push_back(luaL_toreg("makeRotationY", l_makeroty));
  functions.push_back(luaL_toreg("makeRotationZ", l_makerotz));
  functions.push_back(luaL_toreg("makeIdentity", l_makeident));
  functions.push_back(luaL_toreg("mean", l_mean));
  functions.push_back(luaL_toreg("covariance", l_cov));

  return functions;
}
