#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H

#include "filter.h"
#include "accumulator.h"
#include "orthogonalplane.h"

class vtkImageData;
class vtkRunThroughFilter;

namespace udg {

class WindowLevelFilter;
class ThickSlabFilter;
class DisplayShutterFilter;
class TransferFunction;

/// Pipeline that applies the filters required to generate the image to be used in Q2DViewer
class ImagePipeline : public Filter {

public:
    ImagePipeline();
    virtual ~ImagePipeline();

    /// Sets the given vtkImageData as input of the filter
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter
    void setInput(FilterOutput input);

    /// Sets the window window and window level
    bool setWindowLevel(double window, double level);
    /// Gets the window window and window level
    void getCurrentWindowLevel(double wl[]);
    /// Sets the transfer function
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Sets the shutter data
    void setShutterData(vtkImageData *shutterData);
    /// Sets the slice to be visualized
    void setSlice(int slice);
    /// Sets the orthogonal that has to be visualized
    void setProjectionAxis(const OrthogonalPlane &axis);
    /// Sets the slab thickness given by the number of slices
    void setSlabThickness(int numberOfSlices);
    /// Sets the stride between slices
    void setSlabStride(int step);
    /// Sets the projection mode of the accumulation
    void setSlabProjectionMode(AccumulatorFactory::AccumulatorType type);
    /// Returns the output of the thickslab filter. TODO This should be avoided!
    vtkImageData *getSlabProjectionOutput();

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const;

private:
    /// Filter to apply thick slab
    ThickSlabFilter *m_thickSlabProjectionFilter;
    /// Filter to apply a grayscale to volume
    WindowLevelFilter *m_windowLevelLUTFilter;
    /// Filter to show display shutters
    DisplayShutterFilter *m_displayShutterFilter;
    /// Filter to obtain the final output of the pipeline
    vtkRunThroughFilter *m_outputFilter;

    /// Input data
    vtkImageData *m_input;
    /// The shutter data to be applied
    vtkImageData *m_shutterData;

};

}
#endif // IMAGEPIPELINE_H
