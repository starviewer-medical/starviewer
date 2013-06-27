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
