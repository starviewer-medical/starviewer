/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H

#include "filter.h"
#include "accumulator.h"
#include "orthogonalplane.h"

class vtkImageData;
class vtkRunThroughFilter;

namespace udg {

class ThickSlabFilter;
class DisplayShutterFilter;

/// Pipeline that applies the filters required to generate the image to be used in Q2DViewer
class ImagePipeline : public Filter {

public:
    ImagePipeline();
    virtual ~ImagePipeline();

    /// Sets the given vtkImageData as input of the filter
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter
    void setInput(FilterOutput input);

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

    /// Rebuilds this pipeline choosing which filters to use according to their current status.
    void rebuild();

private:
    /// Filter to apply thick slab
    ThickSlabFilter *m_thickSlabProjectionFilter;
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
