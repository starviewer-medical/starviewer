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
    virtual void addValueAttribute(DICOMValueAttribute * attribute);

    /// Afegir una sequencia al fitxer DICOM
    virtual bool addSequenceAttribute(DICOMSequenceAttribute * attribute);

    /// Generar el fitxer
    virtual bool write();

private:

    /// Genera els elements d'una seqüència
    DcmSequenceOfItems * generateDcmSequenceOfItems(DICOMSequenceAttribute * sequenceAttribute);

private:
    DcmFileFormat * m_fileFormat;
};

}

#endif // UDGDICOMWRITERDCMTK_H
