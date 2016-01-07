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
