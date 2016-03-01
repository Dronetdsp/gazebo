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
#ifndef _GAZEBO_PHYSICS_SCREWJOINT_HH_
#define _GAZEBO_PHYSICS_SCREWJOINT_HH_

#include "gazebo/physics/Joint.hh"
#include "gazebo/common/Console.hh"
#include "gazebo/msgs/msgs.hh"
#include "gazebo/util/system.hh"

namespace gazebo
{
  namespace physics
  {
    // Forward declare private data class.
    class ScrewJointPrivate;

    /// \addtogroup gazebo_physics
    /// \{

    /// \class ScrewJoint ScrewJoint.hh physics/physics.hh
    /// \brief A screw joint, which has both  prismatic and rotational DOFs
    template<class T>
    class GZ_PHYSICS_VISIBLE ScrewJoint : public T
    {
      /// \brief Constructor.
      /// \param[in] _parent Parent of the joint.
      public: explicit ScrewJoint(BasePtr _parent) : T(_parent)
      {
        this->AddType(Base::SCREW_JOINT);
      }

      /// \brief Destructor.
      public: virtual ~ScrewJoint()
      {
      }

      // Documentation inherited.
      public: virtual unsigned int AngleCount() const
      {
        return 2;
      }

      /// \brief Load a ScrewJoint.
      /// \param[in] _sdf SDF value to load from
      public: virtual void Load(sdf::ElementPtr _sdf)
      {
        T::Load(_sdf);

        this->threadPitch =
          _sdf->GetElement("thread_pitch")->Get<double>();
      }

      /// \brief Set screw joint thread pitch.
      /// Thread Pitch is defined as angular motion per linear
      /// motion or rad / m in metric.
      /// This must be implemented in a child class
      /// To clarify direction, these are modeling right handed threads
      /// with positive thread_pitch, i.e. the child Link is the nut
      /// (interior threads) while the parent Link is the bolt/screw
      /// (exterior threads).
      /// \param[in] _threadPitch Thread pitch value.
      public: virtual void SetThreadPitch(const double _threadPitch) = 0;

      /// \brief Get screw joint thread pitch.
      /// Thread Pitch is defined as angular motion per linear
      /// motion or rad / m in metric.
      /// This must be implemented in a child class
      /// \return _threadPitch Thread pitch value.
      /// \deprecated See ThreadPitch()
      public: virtual double GetThreadPitch() GAZEBO_DEPRECATED(7.0)
      {
        return this->ThreadPitch();
      }

      /// \brief Get screw joint thread pitch.
      /// Thread Pitch is defined as angular motion per linear
      /// motion or rad / m in metric.
      /// This must be implemented in a child class
      /// \return _threadPitch Thread pitch value.
      public: virtual double ThreadPitch() const = 0;

      // Documentation inherited
      public: virtual void FillMsg(msgs::Joint &_msg)
      {
        Joint::FillMsg(_msg);
        msgs::Joint::Screw *screwMsg = _msg.mutable_screw();
        screwMsg->set_thread_pitch(this->threadPitch);
      }

      /// \brief Initialize joint
      protected: virtual void Init()
      {
        T::Init();
      }

      /// \brief Pitch of the thread.
      protected: double threadPitch = 0.0;
    };
    /// \}
  }
}
#endif
