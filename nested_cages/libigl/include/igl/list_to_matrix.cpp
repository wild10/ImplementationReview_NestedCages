// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "list_to_matrix.h"

#include <cassert>
#include <cstdio>

#include <Eigen/Dense>

#include "max_size.h"
#include "min_size.h"

template <typename T, typename Derived>
IGL_INLINE bool igl::list_to_matrix(const std::vector<std::vector<T > > & V,Eigen::PlainObjectBase<Derived>& M)
{
  // number of rows
  int m = V.size();
  if(m == 0)
  {
    M.resize(0,0);
    return true;
  }
  // number of columns
  int n = igl::min_size(V);
  if(n != igl::max_size(V))
  {
    return false;
  }
  assert(n != -1);
  // Resize output
  M.resize(m,n);

  // Loop over rows
  for(int i = 0;i<m;i++)
  {
    // Loop over cols
    for(int j = 0;j<n;j++)
    {
      M(i,j) = V[i][j];
    }
  }

  return true;
}

template <typename T, typename Derived>
IGL_INLINE bool igl::list_to_matrix(
  const std::vector<std::vector<T > > & V,
  const int n,
  const T & padding,
  Eigen::PlainObjectBase<Derived>& M)
{
  const int m = V.size();
  M.resize(m,n);
  for(int i = 0;i<m;i++)
  {
    const auto & row = V[i];
    if(row.size()>n)
    {
      return false;
    }
    int j = 0;
    for(;j<row.size();j++)
    {
      M(i,j) = row[j];
    }
    for(;j<n;j++)
    {
      M(i,j) = padding;
    }
  }
  return true;
}

template <typename T, typename Derived>
IGL_INLINE bool igl::list_to_matrix(const std::vector<T > & V,Eigen::PlainObjectBase<Derived>& M)
{
  // number of rows
  int m = V.size();
  if(m == 0)
  {
    //fprintf(stderr,"Error: list_to_matrix() list is empty()\n");
    //return false;
    if(Eigen::PlainObjectBase<Derived>::ColsAtCompileTime == 1)
    {
      M.resize(0,1);
    }else if(Eigen::PlainObjectBase<Derived>::RowsAtCompileTime == 1)
    {
      M.resize(1,0);
    }else
    {
      M.resize(0,0);
    }
    return true;
  }
  // Resize output
  if(Eigen::PlainObjectBase<Derived>::RowsAtCompileTime == 1)
  {
    M.resize(1,m);
  }else
  {
    M.resize(m,1);
  }

  // Loop over rows
  for(int i = 0;i<m;i++)
  {
    M(i) = V[i];
  }

  return true;
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template specialization
template bool igl::list_to_matrix<double, Eigen::Matrix<double, -1, 3, 0, -1, 3> >(std::vector<std::vector<double, std::allocator<double> >, std::allocator<std::vector<double, std::allocator<double> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 0, -1, 3> >&);
template bool igl::list_to_matrix<double, Eigen::Matrix<double, -1, 3, 1, -1, 3> >(std::vector<std::vector<double, std::allocator<double> >, std::allocator<std::vector<double, std::allocator<double> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 3, 1, -1, 3> >&);
template bool igl::list_to_matrix<double, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(std::vector<std::vector<double, std::allocator<double> >, std::allocator<std::vector<double, std::allocator<double> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
template bool igl::list_to_matrix<int, Eigen::Matrix<int, -1, 3, 0, -1, 3> >(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 3, 0, -1, 3> >&);
template bool igl::list_to_matrix<int, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);
template bool igl::list_to_matrix<int, Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> >(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, -1, 1, -1, -1> >&);
template bool igl::list_to_matrix<double, Eigen::Matrix<float, -1, 3, 1, -1, 3> >(std::vector<std::vector<double, std::allocator<double> >, std::allocator<std::vector<double, std::allocator<double> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> >&);
template bool igl::list_to_matrix<float, Eigen::Matrix<float, -1, 3, 1, -1, 3> >(std::vector<std::vector<float, std::allocator<float> >, std::allocator<std::vector<float, std::allocator<float> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> >&);
template bool igl::list_to_matrix<unsigned int, Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3> >(std::vector<std::vector<unsigned int, std::allocator<unsigned int> >, std::allocator<std::vector<unsigned int, std::allocator<unsigned int> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3> >&);
template bool igl::list_to_matrix<int, Eigen::Matrix<int, -1, 1, 0, -1, 1> >(std::vector<int, std::allocator<int> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&);
template bool igl::list_to_matrix<unsigned long, Eigen::Matrix<int, -1, 1, 0, -1, 1> >(std::vector<unsigned long, std::allocator<unsigned long> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> >&);
template bool igl::list_to_matrix<float, Eigen::Matrix<float, -1, -1, 0, -1, -1> >(std::vector<std::vector<float, std::allocator<float> >, std::allocator<std::vector<float, std::allocator<float> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, -1, -1, 0, -1, -1> >&);
template bool igl::list_to_matrix<bool, Eigen::Array<bool, -1, 1, 0, -1, 1> >(std::vector<bool, std::allocator<bool> > const&, Eigen::PlainObjectBase<Eigen::Array<bool, -1, 1, 0, -1, 1> >&);
template bool igl::list_to_matrix<bool, Eigen::Array<bool, -1, -1, 0, -1, -1> >(std::vector<std::vector<bool, std::allocator<bool> >, std::allocator<std::vector<bool, std::allocator<bool> > > > const&, Eigen::PlainObjectBase<Eigen::Array<bool, -1, -1, 0, -1, -1> >&);
template bool igl::list_to_matrix<bool, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(std::vector<bool, std::allocator<bool> > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);
template bool igl::list_to_matrix<bool, Eigen::Matrix<int, -1, -1, 0, -1, -1> >(std::vector<std::vector<bool, std::allocator<bool> >, std::allocator<std::vector<bool, std::allocator<bool> > > > const&, Eigen::PlainObjectBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> >&);
template bool igl::list_to_matrix<double, Eigen::Matrix<double, -1, 1, 0, -1, 1> >(std::__1::vector<double, std::__1::allocator<double> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, 1, 0, -1, 1> >&);
#endif
