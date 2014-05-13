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
    /// Processes SCP response status, filling properly DICOMServiceResponseStatus attribute and dumps the proper logs in case of unsuccessful status
    void processServiceClassProviderResponseStatus(int serviceResponseStatusCode, DcmDataset *statusDetail);

private:
    /// Fills class' DICOMServiceResponseStatus attribute with the information obtained from the request
    void fillResponseStatusFromSCP(int serviceResponseStatusCode, DcmDataset *statusDetail);

private:
    /// The response status of the last c-service request
    DICOMServiceResponseStatus m_responseStatus;
};

} // end namespace udg

#endif
