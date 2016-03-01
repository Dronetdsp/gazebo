/*
 * Copyright (C) 2012-2016 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#include <memory>
#include "gazebo/util/system.hh"

/// \file
/// \brief ODE wrapper forward declarations and typedefs
namespace gazebo
{
  namespace physics
  {
    class ODECollision;
    class ODEJoint;
    class ODELink;
    class ODERayShape;
    class ODESurfaceParams;
    class ODEPhysics;

    /// \def ODECollisionPtr
    /// \brief Boost shared point to ODECollision
    typedef std::shared_ptr<ODECollision> ODECollisionPtr;

    /// \def ODEJointPtr
    /// \brief Boost shared point to ODEJoint
    typedef std::shared_ptr<ODEJoint> ODEJointPtr;

    /// \def ODELinkPtr
    /// \brief Boost shared point to ODELink
    typedef std::shared_ptr<ODELink> ODELinkPtr;

    /// \def ODERayShapePtr
    /// \brief Boost shared point to ODERayShape
    typedef std::shared_ptr<ODERayShape> ODERayShapePtr;

    /// \def ODESurfaceParamsPtr
    /// \brief Boost shared pointer to ODESurfaceParams
    typedef std::shared_ptr<ODESurfaceParams> ODESurfaceParamsPtr;

    /// \def ODEPhysicsPtr
    /// \brief Boost shared pointer for ODEPhysics.
    typedef std::shared_ptr<ODEPhysics> ODEPhysicsPtr;
  }
}
