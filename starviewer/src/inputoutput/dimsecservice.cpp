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

#include "dimsecservice.h"

#include "dicomtagreader.h"
#include "dcdatset.h"

namespace udg {

DIMSECService::DIMSECService()
{
}

DIMSECService::~DIMSECService()
{
}

DICOMServiceResponseStatus DIMSECService::getResponseStatus() const
{
    return m_responseStatus;
}

void DIMSECService::setUpAsCFind()
{
    m_responseStatus.setServiceType(DICOMServiceResponseStatus::CFind);
}

void DIMSECService::setUpAsCMove()
{
    m_responseStatus.setServiceType(DICOMServiceResponseStatus::CMove);
}

void DIMSECService::setUpAsCStore()
{
    m_responseStatus.setServiceType(DICOMServiceResponseStatus::CStore);
}

void DIMSECService::setUpAsCGet()
{
    m_responseStatus.setServiceType(DICOMServiceResponseStatus::CGet);
}

void DIMSECService::fillResponseStatusFromSCP(int serviceResponseStatusCode, DcmDataset *statusDetail)
{
    m_responseStatus.clear();
    m_responseStatus.setStatusCode(serviceResponseStatusCode);
    if (statusDetail)
    {
        DICOMTagReader reader(QString(), (DcmDataset*)statusDetail->clone());
        QList<DICOMTag> relatedFields = m_responseStatus.getRelatedFields();
        foreach (const DICOMTag &tag, relatedFields)
        {
            DICOMValueAttribute *value = reader.getValueAttribute(tag);
            m_responseStatus.addRelatedFieldValue(value);
        }
    }
}

void DIMSECService::processServiceClassProviderResponseStatus(int serviceResponseStatusCode, DcmDataset *statusDetail)
{
    fillResponseStatusFromSCP(serviceResponseStatusCode, statusDetail);
    m_responseStatus.dumpLog();
}

} // end namespace udg
