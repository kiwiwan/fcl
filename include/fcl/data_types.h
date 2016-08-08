/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011-2014, Willow Garage, Inc.
 *  Copyright (c) 2014-2016, Open Source Robotics Foundation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Open Source Robotics Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Jia Pan */

#ifndef FCL_DATA_TYPES_H
#define FCL_DATA_TYPES_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include <memory>
#include <Eigen/Dense>
#include <Eigen/StdVector>
#include "fcl/deprecated.h"

namespace fcl
{

typedef /*FCL_DEPRECATED*/ double FCL_REAL;
typedef /*FCL_DEPRECATED*/ std::int64_t  FCL_INT64;
typedef /*FCL_DEPRECATED*/ std::uint64_t FCL_UINT64;
typedef /*FCL_DEPRECATED*/ std::int32_t  FCL_INT32;
typedef /*FCL_DEPRECATED*/ std::uint32_t FCL_UINT32;
// TODO(JS): deprecate these types

using real = double;
using int64 = std::int64_t;
using uint64 = std::uint64_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;

template <typename Scalar>
using Vector2 = Eigen::Matrix<Scalar, 2, 1>;

template <typename Scalar>
using Vector3 = Eigen::Matrix<Scalar, 3, 1>;

template <typename Scalar>
using Vector6 = Eigen::Matrix<Scalar, 6, 1>;

template <typename Scalar>
using Vector7 = Eigen::Matrix<Scalar, 7, 1>;

template <typename Scalar, int N>
using VectorN = Eigen::Matrix<Scalar, N, 1>;

template <typename Scalar>
using VectorX = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

template <typename Scalar>
using Matrix3 = Eigen::Matrix<Scalar, 3, 3>;

template <typename Scalar>
using Quaternion = Eigen::Quaternion<Scalar>;

template <typename Scalar>
using Transform3 = Eigen::Transform<Scalar, 3, Eigen::AffineCompact>;

template <typename Scalar>
using Isometry3 = Eigen::Transform<Scalar, 3, Eigen::Isometry>;

template <typename Scalar>
using Translation3 = Eigen::Translation<Scalar, 3>;

template <typename Scalar>
using AngleAxis = Eigen::AngleAxis<Scalar>;

// float types
using Vector3f = Vector3<float>;
template <int N>
using VectorNf = VectorN<float, N>;
using VectorXf = VectorX<float>;
using Matrix3f = Matrix3<float>;
using Quaternionf = Quaternion<float>;
using Isometry3f = Isometry3<float>;
using Transform3f = Isometry3f;
using Translation3f = Translation3<float>;
using AngleAxisf = AngleAxis<float>;

// double types
using Vector3d = Vector3<double>;
template <int N>
using VectorNd = VectorN<double, N>;
using VectorXd = VectorX<double>;
using Matrix3d = Matrix3<double>;
using Quaterniond = Quaternion<double>;
using Isometry3d = Isometry3<double>;
using Transform3d = Isometry3d;
using Translation3d = Translation3<double>;
using AngleAxisd = AngleAxis<double>;

} // namespace fcl

namespace Eigen {

template <typename _Tp>
using aligned_vector = std::vector<_Tp, Eigen::aligned_allocator<_Tp>>;

template <typename _Key, typename _Tp, typename _Compare = std::less<_Key>>
using aligned_map = std::map<_Key, _Tp, _Compare,
    Eigen::aligned_allocator<std::pair<const _Key, _Tp>>>;

#if EIGEN_VERSION_AT_LEAST(3,2,1)

/// Aligned allocator that is compatible with c++11
// Ref: https://bitbucket.org/eigen/eigen/commits/f5b7700
// TODO: Remove this and use Eigen::aligned_allocator once new version of Eigen
// is released with above commit.
template <class T>
class aligned_allocator_cpp11 : public std::allocator<T>
{
public:
  typedef std::size_t     size_type;
  typedef std::ptrdiff_t  difference_type;
  typedef T*              pointer;
  typedef const T*        const_pointer;
  typedef T&              reference;
  typedef const T&        const_reference;
  typedef T               value_type;

  template <class U>
  struct rebind
  {
    typedef aligned_allocator_cpp11<U> other;
  };

  aligned_allocator_cpp11()
    : std::allocator<T>() {}

  aligned_allocator_cpp11(const aligned_allocator_cpp11& other)
    : std::allocator<T>(other) {}

  template <class U>
  aligned_allocator_cpp11(const aligned_allocator_cpp11<U>& other)
    : std::allocator<T>(other) {}

  ~aligned_allocator_cpp11() {}

  pointer allocate(size_type num, const void* /*hint*/ = 0)
  {
    internal::check_size_for_overflow<T>(num);
    return static_cast<pointer>( internal::aligned_malloc(num * sizeof(T)) );
  }

  void deallocate(pointer p, size_type /*num*/)
  {
    internal::aligned_free(p);
  }
};

template <typename _Tp, typename... _Args>
inline std::shared_ptr<_Tp> make_aligned_shared(_Args&&... __args)
{
  typedef typename std::remove_const<_Tp>::type _Tp_nc;
  return std::allocate_shared<_Tp>(Eigen::aligned_allocator_cpp11<_Tp_nc>(),
                                   std::forward<_Args>(__args)...);
}

#else

template <typename _Tp, typename... _Args>
inline std::shared_ptr<_Tp> make_aligned_shared(_Args&&... __args)
{
  typedef typename std::remove_const<_Tp>::type _Tp_nc;
  return std::allocate_shared<_Tp>(Eigen::aligned_allocator<_Tp_nc>(),
                                   std::forward<_Args>(__args)...);
}

#endif

}  // namespace Eigen

#endif
