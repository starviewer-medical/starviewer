#ifndef BLENDFILTER_H
#define BLENDFILTER_H

#include "filter.h"

class vtkImageBlend;
class vtkImageData;

namespace udg {

///    This filter applies a blending of two given input data sets
class BlendFilter : public Filter {

public:
    BlendFilter();
    virtual ~BlendFilter();

    /// Sets the primary data set
    void setBase(vtkImageData *base);
    /// Sets the secondary data set
    void setOverlay(vtkImageData *overlay);
    /// Sets the contribution of each data set
    void setOverlayOpacity(double opacity);

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const;

private:
    vtkImageBlend* m_imageBlend;

};

}

#endif
