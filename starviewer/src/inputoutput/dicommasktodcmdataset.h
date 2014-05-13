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

    /// Quan consultem/descarreguem al PACS li hem d'indicar a quin nivell es vol fer. En funció dels les propietats emplenades al DICOMMask s'obté el nivell de cerca
    /// descàrrega. Si només hem emplenat propietats d'Study es farà a nivell d'Study, si hem emplenat propietats de Study, Series i Image es farà a nivell de Image
    QString getQueryLevelFromDICOMMask(const DicomMask &dicomMask) const;

};
}

#endif // DICOMMASKTODCMDATASET_H
