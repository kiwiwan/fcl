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

#ifndef FCL_NARROWPHASE_DETAIL_SPHERESPHERE_H
#define FCL_NARROWPHASE_DETAIL_SPHERESPHERE_H

#include "fcl/collision_data.h"

namespace fcl
{

namespace details
{

template <typename Scalar>
bool sphereSphereIntersect(const Sphere<Scalar>& s1, const Transform3<Scalar>& tf1,
                           const Sphere<Scalar>& s2, const Transform3<Scalar>& tf2,
                           std::vector<ContactPoint<Scalar>>* contacts);

template <typename Scalar>
bool sphereSphereDistance(const Sphere<Scalar>& s1, const Transform3<Scalar>& tf1,
                          const Sphere<Scalar>& s2, const Transform3<Scalar>& tf2,
                          Scalar* dist, Vector3<Scalar>* p1, Vector3<Scalar>* p2);

//============================================================================//
//                                                                            //
//                              Implementations                               //
//                                                                            //
//============================================================================//

//==============================================================================
template <typename Scalar>
bool sphereSphereIntersect(const Sphere<Scalar>& s1, const Transform3<Scalar>& tf1,
                           const Sphere<Scalar>& s2, const Transform3<Scalar>& tf2,
                           std::vector<ContactPoint<Scalar>>* contacts)
{
  Vector3<Scalar> diff = tf2.translation() - tf1.translation();
  Scalar len = diff.norm();
  if(len > s1.radius + s2.radius)
    return false;

  if(contacts)
  {
    // If the centers of two sphere are at the same position, the normal is (0, 0, 0).
    // Otherwise, normal is pointing from center of object 1 to center of object 2
    const Vector3<Scalar> normal = len > 0 ? (diff / len).eval() : diff;
    const Vector3<Scalar> point = tf1.translation() + diff * s1.radius / (s1.radius + s2.radius);
    const Scalar penetration_depth = s1.radius + s2.radius - len;
    contacts->push_back(ContactPoint<Scalar>(normal, point, penetration_depth));
  }

  return true;
}

//==============================================================================
template <typename Scalar>
bool sphereSphereDistance(const Sphere<Scalar>& s1, const Transform3<Scalar>& tf1,
                          const Sphere<Scalar>& s2, const Transform3<Scalar>& tf2,
                          Scalar* dist, Vector3<Scalar>* p1, Vector3<Scalar>* p2)
{
  Vector3<Scalar> o1 = tf1.translation();
  Vector3<Scalar> o2 = tf2.translation();
  Vector3<Scalar> diff = o1 - o2;
  Scalar len = diff.norm();
  if(len > s1.radius + s2.radius)
  {
    if(dist) *dist = len - (s1.radius + s2.radius);
    if(p1) *p1 = tf1.inverse(Eigen::Isometry) * (o1 - diff * (s1.radius / len));
    if(p2) *p2 = tf2.inverse(Eigen::Isometry) * (o2 + diff * (s2.radius / len));
    return true;
  }

  if(dist) *dist = -1;
  return false;
}

} // namespace details

} // namespace fcl

#endif
