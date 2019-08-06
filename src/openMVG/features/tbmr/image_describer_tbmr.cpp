// Copyright (c) 2015 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "image_describer_tbmr.hpp"

#include "openMVG/features/akaze/mldb_descriptor.hpp"
#include "openMVG/features/akaze/msurf_descriptor.hpp"
#include "openMVG/features/liop/liop_descriptor.hpp"

// DEBUG
#include "third_party/vectorGraphics/svgDrawer.hpp"
#include "openMVG/image/image_io.hpp"

namespace openMVG {
namespace features {

  void DrawFeatSVG2(std::string fn,
                  openMVG::features::AffineFeatures& regions, int w,
                 int h, std::string svgjpgname) {
  using namespace openMVG::image;
  using namespace svg;

  if (!w || !h) {
    Image<unsigned char> image;
    ReadImage(fn.c_str(), &image);
    if (!w) w = image.Width();
    if (!h) h = image.Height();
  }
  svg::svgDrawer svgStream(w, h);
  if (svgjpgname.empty()) svgjpgname = fn;
  svgStream.drawImage(svgjpgname, w, h);

  for (int i = 0; i < regions.size(); ++i) {
    openMVG::Mat2 M = regions[i].M().cast<double>();
    openMVG::Vec2 x = regions[i].coords().cast<double>();

    auto svd = M.jacobiSvd(Eigen::ComputeFullU);
    double rx = (svd.singularValues().x()), ry = (svd.singularValues().y());

    openMVG::Mat2 U = svd.matrixU();
    svgStream.drawAffine(x.x(), x.y(), rx, ry, U(0, 0), U(0, 1), U(1, 0),
                          U(1, 1), svg::svgStyle().stroke("yellow", 2.0), true);

    svgStream.drawLine(x.x(), x.y(), x.x() + M(0, 0), x.y() + M(1, 0),
                       svg::svgStyle().stroke("red"));
    svgStream.drawLine(x.x(), x.y(), x.x() - M(0, 1), x.y() - M(1, 1),
                       svg::svgStyle().stroke("blue"));
  }

  // Save the SVG file
  std::string filename = fn + ".svg";
  std::ofstream svgFile(filename.c_str());
  if (svgFile.is_open()) {
    svgFile << svgStream.closeSvgFile().str();
    svgFile.close();
  }
}

#pragma region VLFeat

void Extract_Orientations(const openMVG::image::Image<float>& image,
                          openMVG::features::AffineFeatures& feats) {

  // TODO

}

#pragma endregion

void Extract_TBMR(const openMVG::image::Image<unsigned char>& image,
                  openMVG::features::AffineFeatures& feats,
                  openMVG::features::AffineFeatures& feats_dark,
                  const TBMR_Image_describer::Params& params) {
  tbmr::Extract_tbmr(image, feats, std::less<uint8_t>(), params._minimumSize,
                     params._maximumRelativeSize);
  tbmr::Extract_tbmr(image, feats_dark, std::greater<uint8_t>(),
                     params._minimumSize, params._maximumRelativeSize);
}

std::unique_ptr<TBMR_Image_describer_LIOP::Regions_type>
TBMR_Image_describer_LIOP::Describe_TBMR_LIOP(
    const image::Image<unsigned char>& image,
    const image::Image<unsigned char>* mask) {
  auto regions = std::unique_ptr<Regions_type>(new Regions_type);

  if (image.size() == 0) return regions;

  std::vector<features::AffineFeature> feats_dark, feats;
  feats.reserve(5000);
  Extract_TBMR(image, feats, feats_dark, params_);
  feats.insert(feats.end(), feats_dark.begin(), feats_dark.end());  // union

  // Feature masking (remove keypoints if they are masked)
  if (mask) {
    feats.erase(std::remove_if(feats.begin(), feats.end(),
                               [&](const features::AffineFeature& pt) {
                                 if (mask)
                                   return (*mask)(pt.y(), pt.x()) == 0;
                                 else
                                   return false;
                               }),
                feats.end());
  }


  // Init LIOP extractor
  LIOP::Liop_Descriptor_Extractor liop_extractor;

  image::Image<float> imagef;
  imagef.base() = image.base().cast<float>();

  if (bOrientation_) {
    Extract_Orientations(imagef, feats); 
  }

  regions->Features().resize(feats.size());
  regions->Descriptors().resize(feats.size());

#ifdef OPENMVG_USE_OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < static_cast<int>(feats.size()); ++i) {
    auto& tmp = regions->Features()[i] = feats[i];

    float desc[144];
    liop_extractor.extract_affine(imagef, tmp, desc);
    for (int j = 0; j < 144; ++j)
      regions->Descriptors()[i][j] =
          static_cast<unsigned char>(desc[j] * 255.f + .5f);
  }
  return regions;
}

// static
std::unique_ptr<TBMR_Image_describer> TBMR_Image_describer::create(
    const TBMR_Image_describer::Params& params, bool orientation) {
  switch (params._eTBMRDescriptor) {
    case TBMR_LIOP:
      return std::unique_ptr<TBMR_Image_describer>(
          new TBMR_Image_describer_LIOP(params, orientation));
    default:
      return {};
  }
}

}  // namespace features
}  // namespace openMVG
