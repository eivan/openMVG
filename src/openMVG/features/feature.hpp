// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2012, 2013 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENMVG_FEATURES_FEATURE_HPP
#define OPENMVG_FEATURES_FEATURE_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>

#include "openMVG/numeric/eigen_alias_definition.hpp"

namespace openMVG {
namespace features {

/**
 * Base class for Point features.
 * Store position of a feature point.
 */
class PointFeature {

  friend std::ostream& operator<<(std::ostream& out, const PointFeature& obj);
  friend std::istream& operator>>(std::istream& in, PointFeature& obj);

public:
  PointFeature(float x=0.0f, float y=0.0f);

  float x() const;
  float y() const;
  const Vec2f & coords() const;

  float& x();
  float& y();
  Vec2f& coords();

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar (coords_(0), coords_(1));
  }

protected:
  Vec2f coords_;  // (x, y).
};

/**
 * Base class for ScaleInvariant Oriented Point features.
 * Add scale and orientation description to basis PointFeature.
 */
class SIOPointFeature : public PointFeature {

  friend std::ostream& operator<<(std::ostream& out, const SIOPointFeature& obj);
  friend std::istream& operator>>(std::istream& in, SIOPointFeature& obj);

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  ~SIOPointFeature() = default;

  SIOPointFeature(float x=0.0f, float y=0.0f,
                  float scale=0.0f, float orient=0.0f);

  float scale() const;
  float& scale();
  float orientation() const;
  float& orientation();

  bool operator ==(const SIOPointFeature& b) const;

  bool operator !=(const SIOPointFeature& b) const;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar (
      coords_(0), coords_(1),
      scale_,
      orientation_);
  }

protected:
  float scale_;        // In pixels.
  float orientation_;  // In radians.
};

/// Return the coterminal angle between [0;2*PI].
/// Angle value must be in Radian.
float getCoterminalAngle(float angle);

/**
* Base class for Affine "Point" features.
* Add a 2-by-2 matrix description of a Local Affine Frame to the basis PointFeature.
*/
class AffineFeature : public PointFeature {

  friend std::ostream& operator<<(std::ostream& out, const AffineFeature& obj);
  friend std::istream& operator>>(std::istream& in, AffineFeature& obj);

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  virtual ~AffineFeature() = default;

  AffineFeature
  (
    float x = 0.0f,
    float y = 0.0f,
    float m11 = 1.0f,
    float m12 = 0.0f,
    float m21 = 0.0f,
    float m22 = 1.0f
  );

  bool operator ==(const AffineFeature& b) const;

  bool operator !=(const AffineFeature& rhs) const;

  template<class Archive>
  void serialize(Archive& ar)
  {
    ar(
      coords_(0), coords_(1),
      M_(0, 0), M_(0, 1), M_(1, 0), M_(1, 1));
  }

  // inner_points_of_the_affine_feature = { M * v | v \in unit_circle }
  Mat2f& M() { return M_; }

  // inner_points_of_the_affine_feature = { M * v | v \in unit_circle }
  const Mat2f& M() const { return M_; }

  void decompose(float& angleInRadians, float& majoraxissize, float& minoraxissize) const;

protected:
  Mat2f M_;
};

/// Read feats from file
template<typename FeaturesT >
static bool loadFeatsFromFile(
  const std::string & sfileNameFeats,
  FeaturesT & vec_feat)
{
  vec_feat.clear();

  std::ifstream fileIn(sfileNameFeats.c_str());
  if (!fileIn.is_open())
  {
    return false;
  }
  std::copy(
    std::istream_iterator<typename FeaturesT::value_type >(fileIn),
    std::istream_iterator<typename FeaturesT::value_type >(),
    std::back_inserter(vec_feat));
  const bool bOk = !fileIn.bad();
  fileIn.close();
  return bOk;
}

/// Write feats to file
template<typename FeaturesT >
static bool saveFeatsToFile(
  const std::string & sfileNameFeats,
  FeaturesT & vec_feat)
{
  std::ofstream file(sfileNameFeats.c_str());
  if (!file.is_open())
    return false;
  std::copy(vec_feat.begin(), vec_feat.end(),
            std::ostream_iterator<typename FeaturesT::value_type >(file,"\n"));
  const bool bOk = file.good();
  file.close();
  return bOk;
}

/// Export point feature based vector to a matrix [(x,y)'T, (x,y)'T]
template<typename FeaturesT>
void PointsToMat(
  const FeaturesT & vec_feats,
  Mat& m)
{
  m.resize(2, vec_feats.size());
  using ValueT = typename FeaturesT::value_type; // Container type

  size_t i = 0;
  for (typename FeaturesT::const_iterator iter = vec_feats.begin();
    iter != vec_feats.end(); ++iter, ++i)
  {
    const ValueT & feat = *iter;
    m.col(i) << feat.x(), feat.y();
  }
}

} // namespace features
} // namespace openMVG

#endif // OPENMVG_FEATURES_FEATURE_HPP
