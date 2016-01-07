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

#include <vtkSmartPointer.h>

class vtkImageData;

namespace udg {

/**
 * Classe que permet crear un objecte vtkImageData amb les propietats desitjades. De moment sempre amb 1 component escalar només.
 */
class VtkImageDataCreator {

public:

    VtkImageDataCreator();

    /// Assigna l'origen del vtkImageData que es crearà.
    void setOrigin(double origin[3]);
    /// Assigna l'spacing del vtkImageData que es crearà.
    void setSpacing(double spacing[3]);
    /// Crea i retorna l'objecte vtkImageData amb les dimensions donades. Funciona per tots els tipus de dades numèrics.
    template <class T> vtkSmartPointer<vtkImageData> createVtkImageData(int width, int height, int depth, const T *data);

private:

    double m_origin[3];
    double m_spacing[3];

};

} // namespace udg

#endif // VTKIMAGEDATACREATOR_H
