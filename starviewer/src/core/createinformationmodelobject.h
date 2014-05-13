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

#ifndef CREATEINFORMATIONMODELOBJECT_H
#define CREATEINFORMATIONMODELOBJECT_H

namespace udg {

class Patient;
class Study;
class Series;
class Image;
class DICOMTagReader;

/**
    Converteix Objects Dataset de dcmtk a objectes d'Starvieweer
  */
class CreateInformationModelObject {

public:
    /// Crea un patient a partir d'un DICOMTagReader
    static Patient* createPatient(DICOMTagReader *dicomTagReader);

    /// Crea un Study a partir d'un DICOMTagReader
    static Study* createStudy(DICOMTagReader *dicomTagReader);

    /// Crea un Series a partir d'un DICOMTagReader
    static Series* createSeries(DICOMTagReader *dicomTagReader);

    /// Crea un Image a partir d'un DICOMTagReader
    static Image* createImage(DICOMTagReader *dicomTagReader);

};

};  // End namespace udg

#endif
