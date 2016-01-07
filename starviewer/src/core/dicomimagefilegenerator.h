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

#ifndef UDGDICOMIMAGEFILEGENERATOR_H
#define UDGDICOMIMAGEFILEGENERATOR_H

#include "dicomfilegenerator.h"

namespace udg {

class Volume;
class Image;

class DICOMImageFileGenerator : public DICOMFileGenerator {
public:
    DICOMImageFileGenerator();

    ~DICOMImageFileGenerator();

    /// Afegir el volume a partir del qual s'ha de generar els fitxers
    void setInput(Volume *volume);

    /// Mètode encarregat de generar el/s fitxer/s a partir del volume introduït
    /// @pre Cal haver afegit un Volume \sa setInpu
    virtual bool generateDICOMFiles();

private:
    bool generateSecondaryCaptureDICOMFiles();
    void fillGeneralImageInfo(DICOMWriter *writer, Image *image);
    void fillImagePixelInfo(DICOMWriter *writer, Image *image);
    void fillSCInfo(DICOMWriter *writer, Image *image);
    void fillSOPInfo(DICOMWriter *writer, Image *image);

private:
    Volume *m_input;

};

}

#endif // UDGDICOMIMAGEFILEGENERATOR_H
