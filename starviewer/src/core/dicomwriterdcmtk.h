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

#ifndef UDGDICOMWRITERDCMTK_H
#define UDGDICOMWRITERDCMTK_H

#include "dicomwriter.h"

class DcmDataset;
class DcmSequenceOfItems;
class DcmFileFormat;

namespace udg {

/**
    Genera fitxers DICOM utilitzant la llibreria dcmtk.
  */
class DICOMWriterDCMTK : public DICOMWriter {

public:
    DICOMWriterDCMTK();

    ~DICOMWriterDCMTK();

    /// Afegir un nou atribut basic al fitxer DICOM
    virtual void addValueAttribute(DICOMValueAttribute *attribute);

    /// Afegir una sequencia al fitxer DICOM
    virtual bool addSequenceAttribute(DICOMSequenceAttribute *attribute);

    /// Generar el fitxer
    virtual bool write();

private:
    /// Genera els elements d'una seqüència
    DcmSequenceOfItems* generateDcmSequenceOfItems(DICOMSequenceAttribute *sequenceAttribute);

private:
    DcmFileFormat *m_fileFormat;
};

}

#endif // UDGDICOMWRITERDCMTK_H
