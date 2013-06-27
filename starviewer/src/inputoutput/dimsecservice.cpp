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

DICOMServiceResponseStatus DIMSECService::fillResponseStatusFromSCP(int serviceResponseStatusCode, DcmDataset *statusDetail)
{
    DICOMServiceResponseStatus responseStatus;
    responseStatus.setStatusCode(serviceResponseStatusCode);
    if (statusDetail)
    {
        DICOMTagReader reader(QString(), (DcmDataset*)statusDetail->clone());
        QList<DICOMTag> relatedFields = responseStatus.getRelatedFields();
        foreach (const DICOMTag &tag, relatedFields)
        {
            DICOMValueAttribute *value = reader.getValueAttribute(tag);
            responseStatus.addRelatedFieldValue(value);
        }
    }

    return responseStatus;
}

} // end namespace udg
