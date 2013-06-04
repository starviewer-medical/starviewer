#ifndef UDGDEFAULTMEASUREMENTTYPESELECTOR_H
#define UDGDEFAULTMEASUREMENTTYPESELECTOR_H

#include "pixelspacingschemepropertyselector.h"
#include "measurementmanager.h"

namespace udg {

/**
    Class to choose the default measurement type of an image.
    Inherits template algorithm from PixelSpacingSchemePropertySelector and implements its own variant to get the desired result.
 */
class DefaultMeasurementTypeSelector : public PixelSpacingSchemePropertySelector {
public:
    DefaultMeasurementTypeSelector();
    ~DefaultMeasurementTypeSelector();
    
     MeasurementManager::MeasurementType getDefaultMeasurementType(const Image *image);

protected:
    void runPixelSpacingSelector();
    void runImagerPixelSpacingWithMagnificationFactorSelector();
    void runImagerPixelSpacingSelector();
    void runEqualPixelSpacingAndImagerPixelSpacingSelector();
    void runDifferentPixelSpacingAndImagerPixelSpacingSelector();
    void runNoSpacingPresentSelector();

private:
    MeasurementManager::MeasurementType m_defaultMeasurementType;
};

} // End namespace udg

#endif
