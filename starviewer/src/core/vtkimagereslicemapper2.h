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

#ifndef UDG_VTKIMAGERESLICEMAPPER2_H
#define UDG_VTKIMAGERESLICEMAPPER2_H

#include <vtkImageResliceMapper.h>

namespace udg {

/**
 * @brief The VtkImageResliceMapper2 class is a subclass of vtkImageResliceMapper with a few additional methods useful to Starviewer.
 */
class VtkImageResliceMapper2 : public vtkImageResliceMapper
{
public:
    static VtkImageResliceMapper2* New();

    vtkTypeMacro(VtkImageResliceMapper2, vtkImageResliceMapper)

    /// Returns the output of the reslice operation.
    vtkImageData* getResliceOutput();

    /// Returns the slice to world matrix.
    vtkMatrix4x4* getSliceToWorldMatrix() const;

    /// Overriden to set a proper background level before the reslice.
    void Render(vtkRenderer *renderer, vtkImageSlice *prop) override;

    using Superclass::Update;

protected:
    VtkImageResliceMapper2();
    virtual ~VtkImageResliceMapper2();

};

} // namespace udg

#endif // UDG_VTKIMAGERESLICEMAPPER2_H
