// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2015 Pierre Moulon.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENMVG_SFM_SFM_LANDMARK_IO_HPP
#define OPENMVG_SFM_SFM_LANDMARK_IO_HPP

#include "openMVG/sfm/sfm_landmark.hpp"

#include <cereal/cereal.hpp> // Serialization

#include <vector>

template <class Archive>
void openMVG::sfm::Observation::save( Archive & ar) const
{
  ar(cereal::make_nvp("id_feat", id_feat ));
  const std::vector<double> pp{ x(0), x(1) };
  ar(cereal::make_nvp("x", pp));
  const std::vector<double> aa{ M(0,0), M(0,1), M(1,0), M(1,1) };
  ar(cereal::make_nvp("M", aa));
}

template <class Archive>
void openMVG::sfm::Observation::load( Archive & ar)
{
  ar(cereal::make_nvp("id_feat", id_feat ));
  std::vector<double> p(2);
  ar(cereal::make_nvp("x", p));
  x = Eigen::Map<const Vec2>(&p[0]);
  std::vector<double> A(4);
  ar(cereal::make_nvp("M", A));
  M = Eigen::Map<const Mat2>(&A[0]);
}


template <class Archive>
void openMVG::sfm::Landmark::save( Archive & ar) const
{
  const std::vector<double> point { X(0), X(1), X(2) };
  ar(cereal::make_nvp("X", point ));
  const std::vector<double> normal{ N(0), N(1), N(2) };
  ar(cereal::make_nvp("N", normal));
  ar(cereal::make_nvp("observations", obs));
}


template <class Archive>
void openMVG::sfm::Landmark::load( Archive & ar)
{
  std::vector<double> point(3);
  ar(cereal::make_nvp("X", point ));
  X = Eigen::Map<const Vec3>(&point[0]);
  std::vector<double> normal(3);
  ar(cereal::make_nvp("N", normal));
  N = Eigen::Map<const Vec3>(&normal[0]);
  ar(cereal::make_nvp("observations", obs));
}

#endif // OPENMVG_SFM_SFM_LANDMARK_IO_HPP
