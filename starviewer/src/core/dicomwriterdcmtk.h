/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMWRITERDCMTK_H
#define UDGDICOMWRITERDCMTK_H

#include "dicomwriter.h"


namespace udg {

/**
Genera fitxers DICOM utilitzant la llibreria dcmtk.

@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMWriterDCMTK : public DICOMWriter
{
Q_OBJECT
public:
    DICOMWriterDCMTK(QObject *parent = 0);

    ~DICOMWriterDCMTK();

    /// Afegir un nou atribut basic al fitxer DICOM
    virtual void addValueAttribute(DICOMValueAttribute * attribute);

    /// Afegir una sequencia al fitxer DICOM
    virtual void addSequenceAttribute(DICOMSequenceAttribute * attribute);

    /// Generar el fitxer
    virtual bool write();

};

}

#endif // UDGDICOMWRITERDCMTK_H
