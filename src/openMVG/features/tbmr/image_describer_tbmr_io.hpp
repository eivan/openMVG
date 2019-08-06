#ifndef OPENMVG_FEATURES_TBMR_IMAGE_DESCRIBER_IO_HPP
#define OPENMVG_FEATURES_TBMR_IMAGE_DESCRIBER_IO_HPP

#include "openMVG/features/TBMR/image_describer_TBMR.hpp"
#include "openMVG/features/tbmr/tbmr.hpp"

#include <cereal/types/polymorphic.hpp>

/*template<class Archive>
void openMVG::features::TBMR_Image_describer::Params::serialize(Archive & ar)
{
  ar(options_, eTBMRDescriptor_);
}*/

template<class Archive>
void openMVG::features::TBMR_Image_describer::serialize(Archive & ar)
{
  ar(
   cereal::make_nvp("params", params_),
   cereal::make_nvp("bOrientation", bOrientation_));
}


CEREAL_REGISTER_TYPE_WITH_NAME(openMVG::features::TBMR_Image_describer, "TBMR_Image_describer");
CEREAL_REGISTER_POLYMORPHIC_RELATION(openMVG::features::Image_describer, openMVG::features::TBMR_Image_describer)

CEREAL_REGISTER_TYPE_WITH_NAME(openMVG::features::TBMR_Image_describer_LIOP, "TBMR_Image_describer_LIOP");
CEREAL_REGISTER_POLYMORPHIC_RELATION(openMVG::features::Image_describer, openMVG::features::TBMR_Image_describer_LIOP)

#endif // OPENMVG_FEATURES_TBMR_IMAGE_DESCRIBER_IO_HPP
