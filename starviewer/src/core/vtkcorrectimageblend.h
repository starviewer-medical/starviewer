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

#ifndef UDG_VTKCORRECTIMAGEBLEND_H
#define UDG_VTKCORRECTIMAGEBLEND_H

#include <vtkThreadedImageAlgorithm.h>

namespace udg {

/**
 * @brief The VtkCorrectImageBlend class is inspired by the vtkImageBlend class but it does a full blending, including the alpha components of all inputs.
 *
 * Currently it's limited to 32-bit RGBA images.
 */
class VtkCorrectImageBlend : public vtkThreadedImageAlgorithm {

public:
    vtkTypeMacro(VtkCorrectImageBlend, vtkThreadedImageAlgorithm)

    static VtkCorrectImageBlend* New();

protected:
    VtkCorrectImageBlend();
    virtual ~VtkCorrectImageBlend();

    virtual int FillInputPortInformation(int port, vtkInformation *info);
    virtual int RequestUpdateExtent(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    virtual void ThreadedRequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector,
                                     vtkImageData ***inputData, vtkImageData **outputData, int outputExtent[6], int threadId);

private:
    VtkCorrectImageBlend(const VtkCorrectImageBlend&);  // Not implemented.
    void operator=(const VtkCorrectImageBlend&);        // Not implemented.

};

} // namespace udg

#endif // UDG_VTKCORRECTIMAGEBLEND_H
