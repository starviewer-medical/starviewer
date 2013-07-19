#ifndef DISPLAYSHUTTERFILTER_H
#define DISPLAYSHUTTERFILTER_H

#include "filter.h"

class vtkImageData;
class vtkImageMask;

namespace udg {

class Volume;

/**
    This filter applies a display shutter to the input.
 */
class DisplayShutterFilter : public Filter {

public:
    DisplayShutterFilter();
    virtual ~DisplayShutterFilter();

    /// Sets the given volume as input of the filter.
    void setInput(Volume *volume);
    /// Sets the given vtkImageData as input of the filter.
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter.
    void setInput(FilterOutput input);

    /// Sets the display shutter in vtkImageData format.
    void setDisplayShutter(vtkImageData *displayShutter);
    /// Sets the display shutter from the given filter output.
    void setDisplayShutter(FilterOutput displayShutter);

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const;

private:
    /// Image mask that implements the filter.
    vtkImageMask* m_imageMask;

};

}

#endif
