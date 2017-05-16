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

#ifndef VTKIMAGEDATACREATOR_H
#define VTKIMAGEDATACREATOR_H

#include <array>

#include <vtkSmartPointer.h>

class vtkImageData;

namespace udg {

/**
 * @brief The VtkImageDataCreator class allows to create a vtkImageData instance using the builder pattern.
 */
class VtkImageDataCreator {

public:

    VtkImageDataCreator();

    /// Assigna l'origen del vtkImageData que es crearà.
    VtkImageDataCreator& setOrigin(double origin[3]);
    /// Assigna l'spacing del vtkImageData que es crearà.
    VtkImageDataCreator& setSpacing(double spacing[3]);
    /// Sets the dimensions of the vtkImageData to be created.
    VtkImageDataCreator& setDimensions(std::array<int, 3> dimensions);
    /// Sets the number of components of the vtkImageData to be created.
    VtkImageDataCreator& setNumberOfComponents(int numberOfComponents);

    /// Creates and returns a vtkImageData instance with the current configuration and the given data.
    template <class T>
    vtkSmartPointer<vtkImageData> create(const T *data);
    /// Creates and returns a vtkImageData instance with the current origin and spacing, the given dimensions, 1 component, and the given data.
    /// Kept for backward compatibility, you should use create() instead.
    template <class T> vtkSmartPointer<vtkImageData> createVtkImageData(int width, int height, int depth, const T *data);

private:

    double m_origin[3];
    double m_spacing[3];
    std::array<int, 3> m_dimensions;
    int m_numberOfComponents;

};

} // namespace udg

#endif // VTKIMAGEDATACREATOR_H
