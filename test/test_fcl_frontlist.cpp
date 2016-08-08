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

#include <gtest/gtest.h>

#include "fcl/collision_node.h"
#include "test_fcl_utility.h"

#include "fcl_resources/config.h"

using namespace fcl;

template<typename BV>
bool collide_front_list_Test(const Transform3<typename BV::Scalar>& tf1, const Transform3<typename BV::Scalar>& tf2,
                             const std::vector<Vector3<typename BV::Scalar>>& vertices1, const std::vector<Triangle>& triangles1,
                             const std::vector<Vector3<typename BV::Scalar>>& vertices2, const std::vector<Triangle>& triangles2,
                             SplitMethodType split_method,
                             bool refit_bottomup, bool verbose);

template<typename BV, typename TraversalNode>
bool collide_front_list_Test_Oriented(const Transform3<typename BV::Scalar>& tf1, const Transform3<typename BV::Scalar>& tf2,
                                      const std::vector<Vector3<typename BV::Scalar>>& vertices1, const std::vector<Triangle>& triangles1,
                                      const std::vector<Vector3<typename BV::Scalar>>& vertices2, const std::vector<Triangle>& triangles2,
                                      SplitMethodType split_method, bool verbose);


template<typename BV>
bool collide_Test(const Transform3<typename BV::Scalar>& tf,
                  const std::vector<Vector3<typename BV::Scalar>>& vertices1, const std::vector<Triangle>& triangles1,
                  const std::vector<Vector3<typename BV::Scalar>>& vertices2, const std::vector<Triangle>& triangles2, SplitMethodType split_method, bool verbose);

// TODO: randomly still have some runtime error
template <typename Scalar>
void test_front_list()
{
  std::vector<Vector3<Scalar>> p1, p2;
  std::vector<Triangle> t1, t2;

  loadOBJFile(TEST_RESOURCES_DIR"/env.obj", p1, t1);
  loadOBJFile(TEST_RESOURCES_DIR"/rob.obj", p2, t2);

  Eigen::aligned_vector<Transform3<Scalar>> transforms; // t0
  Eigen::aligned_vector<Transform3<Scalar>> transforms2; // t1
  Scalar extents[] = {-3000, -3000, 0, 3000, 3000, 3000};
  Scalar delta_trans[] = {1, 1, 1};
#if FCL_BUILD_TYPE_DEBUG
  std::size_t n = 1;
#else
  std::size_t n = 10;
#endif
  bool verbose = false;

  generateRandomTransforms<Scalar>(extents, delta_trans, 0.005 * 2 * 3.1415, transforms, transforms2, n);

  bool res, res2;

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<AABB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test<AABB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<AABB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test<AABB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<AABB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test<AABB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, false, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<OBB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test<OBB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<OBB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test<OBB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<OBB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test<OBB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, false, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    // Disabled broken test lines. Please see #25.
    // res = collide_Test<RSS<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    // res2 = collide_front_list_Test<RSS<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, false, verbose);
    // EXPECT_TRUE(res == res2);
    res = collide_Test<RSS<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test<RSS<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<RSS<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test<RSS<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, false, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<KDOP<Scalar, 16> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 16> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<KDOP<Scalar, 16> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 16> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<KDOP<Scalar, 16> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 16> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, false, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<KDOP<Scalar, 18> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 18> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<KDOP<Scalar, 18> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 18> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<KDOP<Scalar, 18> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 18> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, false, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<KDOP<Scalar, 24> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 24> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<KDOP<Scalar, 24> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 24> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, false, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<KDOP<Scalar, 24> >(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test<KDOP<Scalar, 24> >(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, false, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<RSS<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test_Oriented<RSS<Scalar>, MeshCollisionTraversalNodeRSS<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<RSS<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test_Oriented<RSS<Scalar>, MeshCollisionTraversalNodeRSS<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<RSS<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test_Oriented<RSS<Scalar>, MeshCollisionTraversalNodeRSS<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    EXPECT_TRUE(res == res2);
  }

  for(std::size_t i = 0; i < transforms.size(); ++i)
  {
    res = collide_Test<OBB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    res2 = collide_front_list_Test_Oriented<OBB<Scalar>, MeshCollisionTraversalNodeOBB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEDIAN, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<OBB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    res2 = collide_front_list_Test_Oriented<OBB<Scalar>, MeshCollisionTraversalNodeOBB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_MEAN, verbose);
    EXPECT_TRUE(res == res2);
    res = collide_Test<OBB<Scalar>>(transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    res2 = collide_front_list_Test_Oriented<OBB<Scalar>, MeshCollisionTraversalNodeOBB<Scalar>>(transforms[i], transforms2[i], p1, t1, p2, t2, SPLIT_METHOD_BV_CENTER, verbose);
    EXPECT_TRUE(res == res2);
  }

}

GTEST_TEST(FCL_FRONT_LIST, front_list)
{
//  test_front_list<float>();
  test_front_list<double>();
}

template<typename BV>
bool collide_front_list_Test(const Transform3<typename BV::Scalar>& tf1, const Transform3<typename BV::Scalar>& tf2,
                             const std::vector<Vector3<typename BV::Scalar>>& vertices1, const std::vector<Triangle>& triangles1,
                             const std::vector<Vector3<typename BV::Scalar>>& vertices2, const std::vector<Triangle>& triangles2,
                             SplitMethodType split_method,
                             bool refit_bottomup, bool verbose)
{
  using Scalar = typename BV::Scalar;

  BVHModel<BV> m1;
  BVHModel<BV> m2;
  m1.bv_splitter.reset(new BVSplitter<BV>(split_method));
  m2.bv_splitter.reset(new BVSplitter<BV>(split_method));

  BVHFrontList front_list;


  std::vector<Vector3<Scalar>> vertices1_new(vertices1.size());
  for(std::size_t i = 0; i < vertices1_new.size(); ++i)
  {
    vertices1_new[i] = tf1 * vertices1[i];
  }

  m1.beginModel();
  m1.addSubModel(vertices1_new, triangles1);
  m1.endModel();

  m2.beginModel();
  m2.addSubModel(vertices2, triangles2);
  m2.endModel();

  Transform3<Scalar> pose1 = Transform3<Scalar>::Identity();
  Transform3<Scalar> pose2 = Transform3<Scalar>::Identity();

  CollisionResult<Scalar> local_result;
  MeshCollisionTraversalNode<BV> node;

  if(!initialize<BV>(node, m1, pose1, m2, pose2,
                     CollisionRequest<Scalar>(std::numeric_limits<int>::max(), false), local_result))
    std::cout << "initialize error" << std::endl;

  node.enable_statistics = verbose;

  collide(&node, &front_list);

  if(verbose) std::cout << "front list size " << front_list.size() << std::endl;


  // update the mesh
  for(std::size_t i = 0; i < vertices1.size(); ++i)
  {
    vertices1_new[i] = tf2 * vertices1[i];
  }

  m1.beginReplaceModel();
  m1.replaceSubModel(vertices1_new);
  m1.endReplaceModel(true, refit_bottomup);

  m2.beginReplaceModel();
  m2.replaceSubModel(vertices2);
  m2.endReplaceModel(true, refit_bottomup);

  local_result.clear();
  collide(&node, &front_list);

  if(local_result.numContacts() > 0)
    return true;
  else
    return false;
}




template<typename BV, typename TraversalNode>
bool collide_front_list_Test_Oriented(const Transform3<typename BV::Scalar>& tf1, const Transform3<typename BV::Scalar>& tf2,
                                      const std::vector<Vector3<typename BV::Scalar>>& vertices1, const std::vector<Triangle>& triangles1,
                                      const std::vector<Vector3<typename BV::Scalar>>& vertices2, const std::vector<Triangle>& triangles2,
                                      SplitMethodType split_method, bool verbose)
{
  using Scalar = typename BV::Scalar;

  BVHModel<BV> m1;
  BVHModel<BV> m2;
  m1.bv_splitter.reset(new BVSplitter<BV>(split_method));
  m2.bv_splitter.reset(new BVSplitter<BV>(split_method));

  BVHFrontList front_list;

  m1.beginModel();
  m1.addSubModel(vertices1, triangles1);
  m1.endModel();

  m2.beginModel();
  m2.addSubModel(vertices2, triangles2);
  m2.endModel();

  Transform3<Scalar> pose1(tf1);
  Transform3<Scalar> pose2 = Transform3<Scalar>::Identity();

  CollisionResult<Scalar> local_result;
  TraversalNode node;

  if(!initialize(node, (const BVHModel<BV>&)m1, pose1, (const BVHModel<BV>&)m2, pose2,
                 CollisionRequest<Scalar>(std::numeric_limits<int>::max(), false), local_result))
    std::cout << "initialize error" << std::endl;

  node.enable_statistics = verbose;

  collide(&node, &front_list);

  if(verbose) std::cout << "front list size " << front_list.size() << std::endl;


  // update the mesh
  pose1 = tf2;
  if(!initialize(node, (const BVHModel<BV>&)m1, pose1, (const BVHModel<BV>&)m2, pose2, CollisionRequest<Scalar>(), local_result))
    std::cout << "initialize error" << std::endl;

  local_result.clear();
  collide(&node, &front_list);

  if(local_result.numContacts() > 0)
    return true;
  else
    return false;
}


template<typename BV>
bool collide_Test(const Transform3<typename BV::Scalar>& tf,
                  const std::vector<Vector3<typename BV::Scalar>>& vertices1, const std::vector<Triangle>& triangles1,
                  const std::vector<Vector3<typename BV::Scalar>>& vertices2, const std::vector<Triangle>& triangles2, SplitMethodType split_method, bool verbose)
{
  using Scalar = typename BV::Scalar;

  BVHModel<BV> m1;
  BVHModel<BV> m2;
  m1.bv_splitter.reset(new BVSplitter<BV>(split_method));
  m2.bv_splitter.reset(new BVSplitter<BV>(split_method));

  m1.beginModel();
  m1.addSubModel(vertices1, triangles1);
  m1.endModel();

  m2.beginModel();
  m2.addSubModel(vertices2, triangles2);
  m2.endModel();

  Transform3<Scalar> pose1(tf);
  Transform3<Scalar> pose2 = Transform3<Scalar>::Identity();

  CollisionResult<Scalar> local_result;
  MeshCollisionTraversalNode<BV> node;

  if(!initialize<BV>(node, m1, pose1, m2, pose2,
                     CollisionRequest<Scalar>(std::numeric_limits<int>::max(), false), local_result))
    std::cout << "initialize error" << std::endl;

  node.enable_statistics = verbose;

  collide(&node);


  if(local_result.numContacts() > 0)
    return true;
  else
    return false;
}

//==============================================================================
int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
