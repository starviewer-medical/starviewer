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

#include "encapsulateddocumentfillerstep.h"

#include "dicomtagreader.h"
#include "encapsulateddocument.h"
#include "patientfillerinput.h"
#include "series.h"

namespace udg {

EncapsulatedDocumentFillerStep::EncapsulatedDocumentFillerStep()
{
}

EncapsulatedDocumentFillerStep::~EncapsulatedDocumentFillerStep()
{
}

bool EncapsulatedDocumentFillerStep::fillIndividually()
{
    const DICOMTagReader *dicomReader = m_input->getDICOMFile();

    if (!dicomReader)
    {
        return false;
    }

    // TODO Support UIDEncapsulatedCDAStorage?
    if (dicomReader->getValueAttributeAsQString(DICOMSOPClassUID) == UIDEncapsulatedPDFStorage)
    {
        EncapsulatedDocument *document = new EncapsulatedDocument();
        document->setTransferSyntaxUid(dicomReader->getValueAttributeAsQString(DICOMTransferSyntaxUID));
        document->setSopInstanceUid(dicomReader->getValueAttributeAsQString(DICOMSOPInstanceUID));
        document->setInstanceNumber(dicomReader->getValueAttributeAsQString(DICOMInstanceNumber));
        document->setDocumentTitle(dicomReader->getValueAttributeAsQString(DICOMDocumentTitle));
        document->setMimeTypeOfEncapsulatedDocument(dicomReader->getValueAttributeAsQString(DICOMMIMETypeOfEncapsulatedDocument));
        document->setPath(dicomReader->getFileName());
        document->setDicomSource(m_input->getDICOMSource());

        m_input->getCurrentSeries()->addEncapsulatedDocument(document);
    }

    return true;
}

} // namespace udg
