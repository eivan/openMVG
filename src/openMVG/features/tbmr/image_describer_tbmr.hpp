#ifndef OPENMVG_FEATURES_TBMR_IMAGE_DESCRIBER_HPP
#define OPENMVG_FEATURES_TBMR_IMAGE_DESCRIBER_HPP

#include "openMVG/features/image_describer.hpp"
#include "openMVG/features/regions_factory.hpp"
#include "openMVG/features/tbmr/tbmr.hpp"

#include <iostream>
#include <numeric>

namespace openMVG {
namespace features {

enum ETBMR_DESCRIPTOR { TBMR_LIOP };

class TBMR_Image_describer : public Image_describer {
 public:
  struct Params {
    Params(ETBMR_DESCRIPTOR eTBMRDescriptor = TBMR_LIOP,
           const unsigned int minimumSize = 30,
           const double maximumRelativeSize = 0.01)
        : _eTBMRDescriptor(eTBMRDescriptor),
          _minimumSize(minimumSize),
          _maximumRelativeSize(maximumRelativeSize) {}

    template <class Archive>
    void serialize(Archive& ar) {
      ar(cereal::make_nvp("eTBMRDescriptor", _eTBMRDescriptor),
         cereal::make_nvp("minimumSize", _minimumSize),
         cereal::make_nvp("maximumRelativeSize", _maximumRelativeSize));
    }

    // Parameters
    ETBMR_DESCRIPTOR _eTBMRDescriptor;
    unsigned int _minimumSize;
    double _maximumRelativeSize;
  };

  TBMR_Image_describer(const Params& params = Params(),
                       bool bOrientation = true)
      : Image_describer(), params_(params), bOrientation_(bOrientation) {}

  static std::unique_ptr<TBMR_Image_describer> create(const Params& params,
                                                      bool orientation = true);

  bool Set_configuration_preset(EDESCRIBER_PRESET preset) override {
    switch (preset) {
      case NORMAL_PRESET:
        params_._maximumRelativeSize = 0.01;
        break;
      case HIGH_PRESET:
        params_._maximumRelativeSize = 0.04;
        break;
      case ULTRA_PRESET:
        params_._minimumSize = 17;
        params_._maximumRelativeSize = 5.5;
        break;
      default:
        return false;
    }
    return true;
  }

  template <class Archive>
  void serialize(Archive& ar);

 protected:
  virtual float GetfDescFactor() const { return 10.f * sqrtf(2.f); }

  Params params_;
  bool bOrientation_;
};

class TBMR_Image_describer_LIOP : public TBMR_Image_describer {
 public:
  using Regions_type = TBMR_Liop_Regions;

  TBMR_Image_describer_LIOP(const Params& params = Params(),
                            bool bOrientation = true)
      : TBMR_Image_describer(params, bOrientation) {}

  std::unique_ptr<Regions> Describe(
      const image::Image<unsigned char>& image,
      const image::Image<unsigned char>* mask = nullptr) override {
    return Describe_TBMR_LIOP(image, mask);
  }

  std::unique_ptr<Regions> Allocate() const override {
    return std::unique_ptr<Regions_type>(new Regions_type);
  }

  std::unique_ptr<Regions_type> Describe_TBMR_LIOP(
      const image::Image<unsigned char>& image,
      const image::Image<unsigned char>* mask = nullptr);
};

void Extract_Orientations(const openMVG::image::Image<float>& image,
                          openMVG::features::AffineFeatures& feats);

}  // namespace features
}  // namespace openMVG

#endif  // OPENMVG_FEATURES_TBMR_IMAGE_DESCRIBER_HPP
