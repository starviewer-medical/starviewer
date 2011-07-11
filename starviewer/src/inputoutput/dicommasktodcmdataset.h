#ifndef DICOMMASKTODCMDATASET_H
#define DICOMMASKTODCMDATASET_H

class DcmDataset;
class DcmTagKey;
class QString;

namespace udg{

class DicomMask;

/// Ens converteix un DicomMask a DcmDataset
class DicomMaskToDcmDataset
{
public:
    /// Ens converteix un DICOMMask a un DcmDataset de Dcmtk per consultar objectes DICOM
    DcmDataset* getDicomMaskAsDcmDataset(const DicomMask &dicomMask);

private:

    /// Ens afegeix el tag amb el seu valor al DcmDataset passat per paràmetre, sempre i quan tagValue no sigui null
    void addTagToDcmDatasetAsString(DcmDataset *dcmDataset, const DcmTagKey &dcmTagKey, const QString &tagValue);

    /// Quan consultem al PACS li hem d'indicar a quin nivell es vol fer. En funció dels les propietats emplenades al DICOMMask s'obté el nivell de cerca.
    /// Si només hem emplenat propietats d'Study es farà a nivell d'Study, si hem emplenat propietats de Study, Series i Image es farà a nivell de Image
    QString getQueryLevelFromDICOMMaskAsQString(const DicomMask &dicomMask) const;

};
}

#endif // DICOMMASKTODCMDATASET_H
