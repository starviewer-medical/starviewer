#ifndef UDGPIXELSPACINGAMENDERPOSTPROCESSOR_H
#define UDGPIXELSPACINGAMENDERPOSTPROCESSOR_H

#include "postprocessor.h"

namespace udg {

/**
    Post processor that amends pixel spacing if the values in Volume differ from the values in its images
 */
class PixelSpacingAmenderPostProcessor : public Postprocessor {
public:
    /// Amends volume's X, Y pixel spacing in case it differs from its images
    virtual void postprocess(Volume *volume);
};

} // namespace udg

#endif // UDGPIXELSPACINGAMENDERPOSTPROCESSOR_H
