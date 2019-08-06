// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2012, 2013 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "openMVG/features/feature.hpp"

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>

namespace openMVG {
namespace features {

PointFeature::PointFeature(float x, float y): coords_(x, y) {}

float PointFeature::x() const { return coords_(0); }
float PointFeature::y() const { return coords_(1); }
const Vec2f & PointFeature::coords() const { return coords_;}

float& PointFeature::x() { return coords_(0); }
float& PointFeature::y() { return coords_(1); }
Vec2f& PointFeature::coords() { return coords_;}


//with overloaded operators:
std::ostream& operator<<(std::ostream& out, const PointFeature& obj)
{
  return out << obj.coords_(0) << " " << obj.coords_(1);
}

std::istream& operator>>(std::istream& in, PointFeature& obj)
{
  return in >> obj.coords_(0) >> obj.coords_(1);
}


SIOPointFeature::SIOPointFeature
(
  float x,
  float y,
  float scale,
  float orient
):
  PointFeature(x,y),
  scale_(scale),
  orientation_(orient)
{

}

float SIOPointFeature::scale() const { return scale_; }
float& SIOPointFeature::scale() { return scale_; }
float SIOPointFeature::orientation() const { return orientation_; }
float& SIOPointFeature::orientation() { return orientation_; }

bool SIOPointFeature::operator ==(const SIOPointFeature& b) const {
  return (scale_ == b.scale()) &&
         (orientation_ == b.orientation()) &&
         (x() == b.x()) && (y() == b.y());
};

bool SIOPointFeature::operator !=(const SIOPointFeature& b) const {
  return !((*this)==b);
};

std::ostream& operator<<(std::ostream& out, const SIOPointFeature& obj)
{
  const PointFeature *pf = static_cast<const PointFeature*>(&obj);
  return out << *pf << " " << obj.scale_ << " " << obj.orientation_;
}

std::istream& operator>>(std::istream& in, SIOPointFeature& obj)
{
  PointFeature *pf = static_cast<PointFeature*>(&obj);
  return in >> *pf >> obj.scale_ >> obj.orientation_;
}

/// Return the coterminal angle between [0;2*PI].
/// Angle value must be in Radian.
float getCoterminalAngle(float angle)
{
  const float f2PI = 2.f*M_PI;
  while (angle > f2PI) {
    angle -= f2PI;
  }
  while (angle < 0.0f) {
    angle += f2PI;
  }
  return angle;
}

AffineFeature::AffineFeature
(
  float x,
  float y,
  float m11,
  float m12,
  float m21,
  float m22
) : PointFeature(x, y), M_((Mat2f() << m11, m12, m21, m22).finished())
{}

bool AffineFeature::operator ==(const AffineFeature& b) const {
  return (x() == b.x() && y() == b.y() && M() == b.M());
};

bool AffineFeature::operator !=(const AffineFeature& rhs) const {
  return !((*this) == rhs);
}

void AffineFeature::decompose(float& angleInRadians, float& majoraxissize, float& minoraxissize) const
{
  auto svd = M().jacobiSvd(Eigen::ComputeFullU);
  angleInRadians = atan2(svd.matrixU()(1, 0), svd.matrixU()(0, 0));
  majoraxissize = svd.singularValues().x();
  minoraxissize = svd.singularValues().y();
}

std::ostream& operator<<(std::ostream& out, const AffineFeature& rhs)
{
  const PointFeature *pf = static_cast<const PointFeature*>(&rhs);
  return out << *pf << " " << rhs.M_(0, 0) << " " << rhs.M_(0, 1) << " " << rhs.M_(1, 0)
    << " " << rhs.M_(1, 1);
}

std::istream& operator>>(std::istream& in, AffineFeature& rhs)
{
  PointFeature *pf = static_cast<PointFeature*>(&rhs);
  return in >> *pf >> rhs.M_(0, 0) >> rhs.M_(0, 1) >> rhs.M_(1, 0) >> rhs.M_(1, 1);
}

} // namespace features
} // namespace openMVG

