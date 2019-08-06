// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2015 Pierre Moulon.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENMVG_SFM_SFM_LANDMARK_HPP
#define OPENMVG_SFM_SFM_LANDMARK_HPP

#include "openMVG/types.hpp"

namespace openMVG {
namespace sfm {

/// Define 3D-2D tracking data: 3D landmark with its 2D observations
struct Observation
{
  Observation():id_feat(UndefinedIndexT) {  }
  Observation(const Vec2& p, const Mat2& A, IndexT idFeat): x(p), M(A), id_feat(idFeat) {}

  // 2D location of the observation (region centre)
  Vec2 x;
  // 2D shape corresponding to the region
  Mat2 M;

  IndexT id_feat;

  // Serialization
  template <class Archive>
  void save( Archive & ar) const;

  // Serialization
  template <class Archive>
  void load( Archive & ar);
};

/// Observations are indexed by their View_id
using Observations = Hash_Map<IndexT, Observation>;

/// Define a landmark (a 3D point, with its 2d observations)
struct Landmark
{
  // 3D location in space
  Vec3 X;
  // Surface normal
  Vec3 N;

  // 2D observations
  Observations obs;

  // Serialization
  template <class Archive>
  void save( Archive & ar) const;

  template <class Archive>
  void load( Archive & ar);
};

/// Define a collection of landmarks are indexed by their TrackId
using Landmarks = Hash_Map<IndexT, Landmark>;

} // namespace sfm
} // namespace openMVG

#endif // OPENMVG_SFM_SFM_LANDMARK_HPP
