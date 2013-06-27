#ifndef UDGDIMSECSERVICE_H
#define UDGDIMSECSERVICE_H

#include "dicomserviceresponsestatus.h"

class DcmDataset;

namespace udg {

/*
    Base class for DIMSE-C Services: C-FIND, C-MOVE, C-STORE, etc. 
 */
class DIMSECService {
public:
    DIMSECService();
    ~DIMSECService();

    /// Gets the response status for the last operation request
    DICOMServiceResponseStatus getResponseStatus() const;

    /// Sets up the class for the intended service. Should be invoked at initialization by any inherited class.
    void setUpAsCFind();
    void setUpAsCMove();
    void setUpAsCStore();
    void setUpAsCGet();
    
protected:
    /// Returns a DICOMServiceResponseStatus object with the information obtained from the request
    DICOMServiceResponseStatus fillResponseStatusFromSCP(int serviceResponseStatusCode, DcmDataset *statusDetail);

protected:
    /// The response status of the last c-service request
    DICOMServiceResponseStatus m_responseStatus;
};

} // end namespace udg

#endif
