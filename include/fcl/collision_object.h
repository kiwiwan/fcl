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


#ifndef FCL_COLLISION_OBJECT_H
#define FCL_COLLISION_OBJECT_H

#include <memory>

#include "fcl/collision_geometry.h"

namespace fcl
{

/// @brief the object for collision or distance computation, contains the
/// geometry and the transform information
template <typename Scalar>
class CollisionObject
{
public:
 CollisionObject(const std::shared_ptr<CollisionGeometry<Scalar>> &cgeom_) :
    cgeom(cgeom_), cgeom_const(cgeom_), t(Transform3<Scalar>::Identity())
  {
    if (cgeom)
    {
      cgeom->computeLocalAABB();
      computeAABB();
    }
  }

  CollisionObject(const std::shared_ptr<CollisionGeometry<Scalar>> &cgeom_, const Transform3<Scalar>& tf) :
    cgeom(cgeom_), cgeom_const(cgeom_), t(tf)
  {
    cgeom->computeLocalAABB();
    computeAABB();
  }

  CollisionObject(const std::shared_ptr<CollisionGeometry<Scalar>> &cgeom_, const Matrix3<Scalar>& R, const Vector3<Scalar>& T):
      cgeom(cgeom_), cgeom_const(cgeom_), t(Transform3<Scalar>::Identity())
  {
    t.linear() = R;
    t.translation() = T;
    cgeom->computeLocalAABB();
    computeAABB();
  }

  ~CollisionObject()
  {
  }

  /// @brief get the type of the object
  OBJECT_TYPE getObjectType() const
  {
    return cgeom->getObjectType();
  }

  /// @brief get the node type
  NODE_TYPE getNodeType() const
  {
    return cgeom->getNodeType();
  }

  /// @brief get the AABB in world space
  const AABB<Scalar>& getAABB() const
  {
    return aabb;
  }

  /// @brief compute the AABB in world space
  void computeAABB()
  {
    if(t.linear().isIdentity())
    {
      aabb = translate(cgeom->aabb_local, t.translation());
    }
    else
    {
      Vector3<Scalar> center = t * cgeom->aabb_center;
      Vector3<Scalar> delta = Vector3<Scalar>::Constant(cgeom->aabb_radius);
      aabb.min_ = center - delta;
      aabb.max_ = center + delta;
    }
  }

  /// @brief get user data in object
  void* getUserData() const
  {
    return user_data;
  }

  /// @brief set user data in object
  void setUserData(void *data)
  {
    user_data = data;
  }

  /// @brief get translation of the object
  const Vector3<Scalar> getTranslation() const
  {
    return t.translation();
  }

  /// @brief get matrix rotation of the object
  const Matrix3<Scalar> getRotation() const
  {
    return t.linear();
  }

  /// @brief get quaternion rotation of the object
  const Quaternion<Scalar> getQuatRotation() const
  {
    return Quaternion<Scalar>(t.linear());
  }

  /// @brief get object's transform
  const Transform3<Scalar>& getTransform() const
  {
    return t;
  }

  /// @brief set object's rotation matrix
  void setRotation(const Matrix3<Scalar>& R)
  {
    t.linear() = R;
  }

  /// @brief set object's translation
  void setTranslation(const Vector3<Scalar>& T)
  {
    t.translation() = T;
  }

  /// @brief set object's quatenrion rotation
  void setQuatRotation(const Quaternion<Scalar>& q)
  {
    t.linear() = q.toRotationMatrix();
  }

  /// @brief set object's transform
  void setTransform(const Matrix3<Scalar>& R, const Vector3<Scalar>& T)
  {
    setRotation(R);
    setTranslation(T);
  }

  /// @brief set object's transform
  void setTransform(const Quaternion<Scalar>& q, const Vector3<Scalar>& T)
  {
    setQuatRotation(q);
    setTranslation(T);
  }

  /// @brief set object's transform
  void setTransform(const Transform3<Scalar>& tf)
  {
    t = tf;
  }

  /// @brief whether the object is in local coordinate
  bool isIdentityTransform() const
  {
    return t.matrix().isIdentity();
  }

  /// @brief set the object in local coordinate
  void setIdentityTransform()
  {
    t.setIdentity();
  }

  /// @brief get geometry from the object instance
  FCL_DEPRECATED
  const CollisionGeometry<Scalar>* getCollisionGeometry() const
  {
    return cgeom.get();
  }

  /// @brief get geometry from the object instance
  const std::shared_ptr<const CollisionGeometry<Scalar>>& collisionGeometry() const
  {
    return cgeom_const;
  }

  /// @brief get object's cost density
  Scalar getCostDensity() const
  {
    return cgeom->cost_density;
  }

  /// @brief set object's cost density
  void setCostDensity(Scalar c)
  {
    cgeom->cost_density = c;
  }

  /// @brief whether the object is completely occupied
  bool isOccupied() const
  {
    return cgeom->isOccupied();
  }

  /// @brief whether the object is completely free
  bool isFree() const
  {
    return cgeom->isFree();
  }

  /// @brief whether the object is uncertain
  bool isUncertain() const
  {
    return cgeom->isUncertain();
  }

protected:

  std::shared_ptr<CollisionGeometry<Scalar>> cgeom;
  std::shared_ptr<const CollisionGeometry<Scalar>> cgeom_const;

  Transform3<Scalar> t;

  /// @brief AABB<Scalar> in global coordinate
  mutable AABB<Scalar> aabb;

  /// @brief pointer to user defined data specific to this object
  void *user_data;

public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

using CollisionObjectf = CollisionObject<float>;
using CollisionObjectd = CollisionObject<double>;

} // namespace fcl

#endif
