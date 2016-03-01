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
#include <functional>
#include "gazebo/physics/Inertial.hh"

using namespace gazebo;
using namespace physics;

sdf::ElementPtr InertialPrivate::sdfInertial;

//////////////////////////////////////////////////
Inertial::Inertial()
{
  this->dataPtr->mass = 1;
  this->dataPtr->principals.Set(1, 1, 1);
  this->dataPtr->products.Set(0, 0, 0);

  if (!this->dataPtr->sdfInertial)
  {
    this->dataPtr->sdfInertial.reset(new sdf::Element);
    initFile("inertial.sdf", this->dataPtr->sdfInertial);
  }

  // This is the only time this->dataPtr->sdfInertial should be used.
  this->dataPtr->sdf = this->dataPtr->sdfInertial->Clone();
}

//////////////////////////////////////////////////
Inertial::Inertial(const double _m)
{
  this->dataPtr->sdf.reset(new sdf::Element);
  initFile("inertial.sdf", this->dataPtr->sdf);

  this->dataPtr->mass = _m;
  this->dataPtr->cog.Set(0, 0, 0, 0, 0, 0);
  this->dataPtr->principals.Set(1, 1, 1);
  this->dataPtr->products.Set(0, 0, 0);
}

//////////////////////////////////////////////////
Inertial::Inertial(const Inertial &_inertial)
{
  this->dataPtr->sdf.reset(new sdf::Element);
  initFile("inertial.sdf", this->dataPtr->sdf);

  (*this) = _inertial;
}

//////////////////////////////////////////////////
Inertial::~Inertial()
{
  this->dataPtr->sdf.reset();
}

//////////////////////////////////////////////////
void Inertial::Load(sdf::ElementPtr _sdf)
{
  this->UpdateParameters(_sdf);
}

//////////////////////////////////////////////////
void Inertial::UpdateParameters(sdf::ElementPtr _sdf)
{
  this->dataPtr->sdf = _sdf;

  // use default pose (identity) if not specified in sdf
  ignition::math::Pose3d pose =
    this->dataPtr->sdf->Get<ignition::math::Pose3d>("pose");
  this->SetCoG(pose);

  // if (this->dataPtr->sdf->HasElement("inertia"))
  // Do the following whether an inertia element was specified or not.
  // Otherwise SetUpdateFunc won't get called.
  {
    sdf::ElementPtr inertiaElem = this->dataPtr->sdf->GetElement("inertia");
    this->SetInertiaMatrix(
        inertiaElem->Get<double>("ixx"),
        inertiaElem->Get<double>("iyy"),
        inertiaElem->Get<double>("izz"),
        inertiaElem->Get<double>("ixy"),
        inertiaElem->Get<double>("ixz"),
        inertiaElem->Get<double>("iyz"));

    inertiaElem->GetElement("ixx")->GetValue()->SetUpdateFunc(
        std::bind(&Inertial::GetIXX, this));
    inertiaElem->GetElement("iyy")->GetValue()->SetUpdateFunc(
        std::bind(&Inertial::GetIYY, this));
    inertiaElem->GetElement("izz")->GetValue()->SetUpdateFunc(
        std::bind(&Inertial::GetIZZ, this));
    inertiaElem->GetElement("ixy")->GetValue()->SetUpdateFunc(
        std::bind(&Inertial::GetIXY, this));
    inertiaElem->GetElement("ixz")->GetValue()->SetUpdateFunc(
        std::bind(&Inertial::GetIXZ, this));
    inertiaElem->GetElement("iyz")->GetValue()->SetUpdateFunc(
        std::bind(&Inertial::GetIYZ, this));
  }

  this->SetMass(this->dataPtr->sdf->Get<double>("mass"));
  this->dataPtr->sdf->GetElement("mass")->GetValue()->SetUpdateFunc(
      std::bind(&Inertial::Mass, this));
}

//////////////////////////////////////////////////
Inertial Inertial::GetInertial(const math::Pose &_frameOffset) const
{
  return (*this)(_frameOffset.Ign());
}

//////////////////////////////////////////////////
Inertial Inertial::operator()(const ignition::math::Pose3d &_frameOffset) const

  // make a copy of the current Inertial
  Inertial result(*this);

  // new CoG location after link frame offset
  result.Cog() = result.Cog() - _frameOffset;

  // new MOI after link frame offset
  result.SetMOI(this->MOI(result.cog));

  return result;
}

//////////////////////////////////////////////////
void Inertial::Reset()
{
  sdf::ElementPtr inertiaElem = this->dataPtr->sdf->GetElement("inertia");

  this->dataPtr->mass = this->dataPtr->sdf->Get<double>("mass");
  this->dataPtr->cog.Set(0, 0, 0, 0, 0, 0);
  this->SetInertiaMatrix(
        inertiaElem->Get<double>("ixx"),
        inertiaElem->Get<double>("iyy"),
        inertiaElem->Get<double>("izz"),

        inertiaElem->Get<double>("ixy"),
        inertiaElem->Get<double>("ixz"),
        inertiaElem->Get<double>("iyz"));
}

//////////////////////////////////////////////////
void Inertial::SetMass(const double _m)
{
  this->dataPtr->mass = _m;
}

//////////////////////////////////////////////////
double Inertial::GetMass() const
{
  return this->Mass();
}

//////////////////////////////////////////////////
double Inertial::Mass() const
{
  return this->dataPtr->mass;
}

//////////////////////////////////////////////////
void Inertial::SetCoG(const double _cx, const double _cy, const double _cz)
{
  this->dataPtr->cog.pos.Set(_cx, _cy, _cz);
}

//////////////////////////////////////////////////
void Inertial::SetCoG(const math::Vector3 &_c)
{
  return this->SetCog(_c.Ign());
}

//////////////////////////////////////////////////
void Inertial::SetCoG(const ignition::math::Vector3d &_c)

{
  this->dataPtr->cog.Pos() = _c;
}

//////////////////////////////////////////////////
void Inertial::SetCoG(const double _cx, const double _cy, const double _cz,
                      const double _rx, const double _ry, const double _rz)
{
  this->dataPtr->cog.Set(_cx, _cy, _cz, _rx, _ry, _rz);
}

//////////////////////////////////////////////////
void Inertial::SetCoG(const math::Pose &_c)
{
  this->SetCoG(_c.Ign());
}

//////////////////////////////////////////////////
void Inertial::SetCoG(const ignition::math::Pose3d &_c)
{
  this->dataPtr->cog = _c;
}

//////////////////////////////////////////////////
void Inertial::SetInertiaMatrix(
    const double ixx, const double iyy, const double izz,
    const double ixy, const double ixz, const double iyz)
{
{
  this->dataPtr->principals.Set(ixx, iyy, izz);
  this->dataPtr->products.Set(ixy, ixz, iyz);
}


//////////////////////////////////////////////////
math::Vector3 Inertial::GetPrincipalMoments() const
{
  return this->PrincipalMoments();
}

//////////////////////////////////////////////////
ignition::math::Vector3d Inertial::PrincipalMoments() const
{
  return this->dataPtr->principals;
}

//////////////////////////////////////////////////
math::Vector3 Inertial::GetProductsofInertia() const
{
  return this->ProductsOfInertia();
}

//////////////////////////////////////////////////
ignition::math::Vector3d Inertial::ProductsOfInertia() const
{
  return this->dataPtr->products;
}

//////////////////////////////////////////////////
void Inertial::SetMOI(const math::Matrix3 &_moi)
{
  /// \TODO: check symmetry of incoming _moi matrix
  this->dataPtr->principals.Set(_moi[0][0], _moi[1][1], _moi[2][2]);
  this->dataPtr->products.Set(_moi[0][1], _moi[0][2], _moi[1][2]);
}

//////////////////////////////////////////////////
math::Matrix3 Inertial::GetMOI() const
{
  return this->MOI();
}

//////////////////////////////////////////////////
ignition::math::Matrix3d Inertial:MOI() const
{
  return ignition::math::Matrix3d(
    this->dataPtr->principals.X(), this->dataPtr->products.X(),   this->dataPtr->products.Y(),
    this->dataPtr->products.X(),   this->dataPtr->principals.Y(), this->dataPtr->products.Z(),
    this->dataPtr->products.Y(),   this->dataPtr->products.Z(),   this->dataPtr->principals.Z());
}

//////////////////////////////////////////////////
void Inertial::Rotate(const math::Quaternion &_rot)
{
  return this->Rotate(_rot.Ign());
}

//////////////////////////////////////////////////
void Inertial::Rotate(const ignition::math::Quaterniond &_rot)
{
  /// \TODO: double check what this does, if needed
  this->dataPtr->cog.Pos() = _rot.RotateVector(this->dataPtr->cog.Pos());
  this->dataPtr->cog.Rot() = _rot * this->dataPtr->cog.Rot();
}

//////////////////////////////////////////////////
Inertial &Inertial::operator=(const Inertial &_inertial)
{
  this->dataPtr->mass = _inertial.mass;
  this->dataPtr->cog = _inertial.cog;
  this->dataPtr->principals = _inertial.principals;
  this->dataPtr->products = _inertial.products;

  return *this;
}

//////////////////////////////////////////////////
Inertial Inertial::operator+(const Inertial &_inertial) const
{
  Inertial result(*this);

  // update mass with sum
  result.mass = this->dataPtr->mass + _inertial.mass;

  // compute new center of mass
  result.cog.Pos() =
    (this->dataPtr->cog.Pos()*this->dataPtr->mass + _inertial.cog.Pos() * _inertial.mass) /
    result.mass;

  // make a decision on the new orientation, set it to identity
  result.cog.Rot() = ignition::math::Quaterniond(1, 0, 0, 0);

  // compute equivalent I for (*this) at the new CoG
  ignition::math::Matrix3d Ithis = this->MOI(result.cog());

  // compute equivalent I for _inertial at the new CoG
  ignition::math::Matrix3d Iparam = _inertial.MOI(result.cog());

  // sum up principals and products now they are at the same location
  result.SetMOI(Ithis + Iparam);

  return result;
}

//////////////////////////////////////////////////
math::Matrix3 Inertial::GetMOI(const math::Pose &_pose) const
{
  return this->MOI();
}

//////////////////////////////////////////////////
ignition::math::Matrix3d Inertial::MOI(
    const ignition::math::Pose3d &_pose) const
{
  // get MOI as a Matrix3
  ignition::math::Matrix3d moi = this->MOI();

  // transform from new _pose to old this->dataPtr->cog, specified in new _pose frame
  ignition::math::Pose3d new2Old = this->dataPtr->cog - _pose;

  // rotate moi into new cog frame
  moi = ignition::math::Matrix3d(new2Old.Rot()) * moi *
        ignition::math::Matrix3d(new2Old.Rot().Inverse());

  // parallel axis theorem to get MOI at the new cog location
  // integrating point mass at some offset
  ignition::math::Vector3d offset = new2Old.Pos();
  moi[0][0] += (offset.Y() * offset.Y() + offset.Z() * offset.Z()) * this->dataPtr->mass;
  moi[0][1] -= (offset.X() * offset.Y()) * this->dataPtr->mass;
  moi[0][2] -= (offset.X() * offset.Z()) * this->dataPtr->mass;
  moi[1][0] -= (offset.Y() * offset.X()) * this->dataPtr->mass;
  moi[1][1] += (offset.X() * offset.X() + offset.Z() * offset.Z()) * this->dataPtr->mass;
  moi[1][2] -= (offset.Y() * offset.Z()) * this->dataPtr->mass;
  moi[2][0] -= (offset.Z() * offset.X()) * this->dataPtr->mass;
  moi[2][1] -= (offset.Z() * offset.Y()) * this->dataPtr->mass;
  moi[2][2] += (offset.X() * offset.X() + offset.Y() * offset.Y()) * this->dataPtr->mass;

  return moi;
}

//////////////////////////////////////////////////
const Inertial &Inertial::operator+=(const Inertial &_inertial)
{
  *this = *this + _inertial;
  return *this;
}

//////////////////////////////////////////////////
double Inertial::GetIXX() const
{
  return this->IXX();
}

//////////////////////////////////////////////////
double Inertial::IXX() const
{
  return this->dataPtr->principals.X();
}

//////////////////////////////////////////////////
double Inertial::GetIYY() const
{
  return this->IYY();
}

//////////////////////////////////////////////////
double Inertial::IYY() const
{
  return this->dataPtr->principals.Y();
}

//////////////////////////////////////////////////
double Inertial::GetIZZ() const
{
  return this->IZZ();
}

//////////////////////////////////////////////////
double Inertial::GetIZZ() const
{
  return this->dataPtr->principals.Z();
}

//////////////////////////////////////////////////
double Inertial::GetIXY() const
{
  return this->IXY();
}

//////////////////////////////////////////////////
double Inertial::IXY() const
{
  return this->dataPtr->products.X();
}

//////////////////////////////////////////////////
double Inertial::GetIXZ() const
{
  return this->IXZ();
}

//////////////////////////////////////////////////
double Inertial::IXZ() const
{
  return this->dataPtr->products.Y();
}

//////////////////////////////////////////////////
double Inertial::GetIYZ() const
{
  return this->IYZ();
}

//////////////////////////////////////////////////
double Inertial::IYZ() const
{
  return this->dataPtr->products.Z();
}

//////////////////////////////////////////////////
void Inertial::SetIXX(const double _v)
{
  this->dataPtr->principals.X(_v);
}

//////////////////////////////////////////////////
void Inertial::SetIYY(const double _v)
{
  this->dataPtr->principals.Y(_v);
}

//////////////////////////////////////////////////
void Inertial::SetIZZ(const double _v)
{
  this->dataPtr->principals.Z(_v);
}

//////////////////////////////////////////////////
void Inertial::SetIXY(const double _v)
{
  this->dataPtr->products.X(_v);
}

//////////////////////////////////////////////////
void Inertial::SetIXZ(const double _v)
{
  this->dataPtr->products.Y(_v);
}

//////////////////////////////////////////////////
void Inertial::SetIYZ(const double _v)
{
  this->dataPtr->products.Z(_v);
}

//////////////////////////////////////////////////
void Inertial::ProcessMsg(const msgs::Inertial &_msg)
{
  if (_msg.has_mass())
    this->SetMass(_msg.mass());
  if (_msg.has_pose())
    this->SetCoG(_msg.pose().position().x(),
                 _msg.pose().position().y(),
                 _msg.pose().position().z());
  if (_msg.has_ixx())
    this->SetIXX(_msg.ixx());
  if (_msg.has_ixy())
    this->SetIXY(_msg.ixy());
  if (_msg.has_ixz())
    this->SetIXZ(_msg.ixz());
  if (_msg.has_iyy())
    this->SetIYY(_msg.iyy());
  if (_msg.has_iyz())
    this->SetIYZ(_msg.iyz());
  if (_msg.has_izz())
    this->SetIZZ(_msg.izz());
}

//////////////////////////////////////////////////
const math::Vector3 &Inertial::GetCoG() const
{
  return this->CoG();
}

//////////////////////////////////////////////////
const ignition::math::Vector3d &Inertial::CoG() const
{
  return this->dataPtr->cog.Pos();
}

//////////////////////////////////////////////////
const math::Pose Inertial::GetPose() const
{
  return this->Pose();
}

//////////////////////////////////////////////////
const ignition::math::Pose3d Inertial::Pose() const
{
  return ignition::math::Pose3d(this->dataPtr->cog);
}
