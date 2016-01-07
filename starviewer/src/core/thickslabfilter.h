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

#ifndef THICKSLABFILTER_H
#define THICKSLABFILTER_H

#include "filter.h"
#include "orthogonalplane.h"

#include "accumulator.h"

class vtkImageData;
class vtkProjectionImageFilter;

namespace udg {

class ThickSlabFilter : public Filter {

public:
    ThickSlabFilter();
    virtual ~ThickSlabFilter();

    /// Sets the given vtkImageData as input of the filter
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter
    void setInput(FilterOutput input);


    void setProjectionAxis(const OrthogonalPlane &axis);
    /// Sets the first slice
    void setFirstSlice(int slice);
    /// Sets the thickness (number of slices)
    void setSlabThickness(int numberOfSlices);
    /// Sets the stride
    void setStride(int stride);
    /// Sets the accumulator type
    void setAccumulatorType(AccumulatorFactory::AccumulatorType type);

    /// Get the thickness
    int getSlabThickness();

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const;

private:
    vtkProjectionImageFilter* m_filter;

};

}

#endif
