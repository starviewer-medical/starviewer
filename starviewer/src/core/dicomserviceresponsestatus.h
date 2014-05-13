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

#ifndef UDGDICOMRESPONSESTATUS_H
#define UDGDICOMRESPONSESTATUS_H

#include <QString>
#include <QList>

#include "dicomtag.h"
#include "dicomvalueattribute.h"

namespace udg {

/**
    Represents the DICOM response status that could be obtained from DIMSE-C service requests as C-GET, C-FIND, C-MOVE and C-STORE
    For more information, see PS 3.4 Annex C.4 and PS 3.7 Annex C - Status Type Enconding
 */
class DICOMServiceResponseStatus {
public:
    DICOMServiceResponseStatus();
    ~DICOMServiceResponseStatus();
    
    /// Status Codes
    // Failure
    static const unsigned int RefusedOutOfResources; // C-FIND & C-STORE
    static const unsigned int RefusedOutOfResourcesUnableToCalculateNumberOfMatches; // C-MOVE & C-GET
    static const unsigned int RefusedOutOfResourcesUnableToPerformSubOperations; // C-MOVE & C-GET
    static const unsigned int RefusedMoveDestinationUnknown; // C-MOVE
    static const unsigned int IdentifierDoesNotMatchSOPClass; // C-FIND, C-MOVE, C-GET & C-STORE
    static const unsigned int ErrorDataSetDoesNotMatchSOPClass; // C-STORE
    static const unsigned int ErrorCannotUnderstand; // C-STORE
    static const unsigned int UnableToProcess; // C-FIND, C-MOVE, C-GET
    // Cancel
    static const unsigned int TerminatedDueToCancelRequest; // C-FIND (Matching terminated..), C-MOVE, C-GET (Suboperations terminated...)
    // Warning
    static const unsigned int WarningSubOperationsCompleteOneOrMoreFailures; // C-MOVE & C-GET
    static const unsigned int WarningCoercionOfDataElements; // C-STORE
    static const unsigned int WarningDatasetDoesNotMatchSOPClass; // C-STORE
    static const unsigned int WarningElementsDiscarded; // C-STORE
    // Pending
    static const unsigned int Pending; // C-FIND, C-MOVE, C-GET
    static const unsigned int PendingWarningUnsupportedOptionalKeys; // C-FIND
    // Success
    static const unsigned int Success; // C-FIND, C-MOVE, C-GET & C-STORE;
    
    /// Classification of the different possible status
    enum ServiceStatusType { FailureStatus, CancelStatus, WarningStatus, SuccessStatus, PendingStatus, UnknownStatus };
    
    /// Kinds of services the response could belong to
    enum ServiceType { CFind, CMove, CGet, CStore, UnknownService };

    /// Set/get the status code
    void setStatusCode(unsigned int code);
    unsigned int getStatusCode() const;

    /// Set/get the service the response belongs to
    void setServiceType(ServiceType service);
    ServiceType getServiceType() const;

    /// Gets the status code as a human-readable string
    QString getStatusCodeAsString() const;
    
    /// Returns the service status type corresponding to the current status code
    ServiceStatusType getServiceStatus() const;

    /// Returns the list of related DICOM field tags for the current status code
    QList<DICOMTag> getRelatedFields();
    
    /// Adds a related field value
    void addRelatedFieldValue(DICOMValueAttribute *value);

    /// Returns the status details in human-readable formatted string
    QString toString() const;

    /// Resets the object to its initial values
    void clear();

    /// Dumps status to corresponding log level
    void dumpLog();

private:
    /// Gets service type as string
    QString getServiceTypeAsString() const;

private:
    /// Status code
    unsigned int m_statusCode;
    
    /// Service type which response belongs to
    ServiceType m_serviceType;

    /// Related fields values
    QList<DICOMValueAttribute*> m_relatedFieldsValues;
};

} // end namespace udg

#endif


/*
// C-STORE
    // a) Refused: Out of Resources: This indicates that the peer DIMSE-service-user was unable to store 
    //      the composite SOP Instance because it was out of resources.
    // b) Refused: SOP Class Not Supported: This indicates that the peer DIMSE-service-user was unable to store 
    //      the composite SOP Instance because the SOP Class is not supported,
    // c) Error: Cannot Understand: This indicates that the peer DIMSE-service-user was unable to store the 
    //      composite SOP Instance because it cannot understand certain Data Elements.
    // d) Error: Data Set does not match SOP Class: This indicates that the peer DIMSE-service-user
    //      was unable to store the composite SOP Instance because the Data Set does not match the SOP Class.
    // e) Warning: This indicates that the peer DIMSE-service-user was able to store the composite
    //      SOP Instance, but detected a probable error.
    // f) Success: This indicates that the composite SOP Instance was successfully stored.
    enum CStoreStatusType { RefusedOutOfResources, RefusedSOPClassNotSupported, ErrorCannotUndertand, ErrorDataSetDoesNotMatchSOPClass, Warning, Success };

    // C-FIND
    // a) Success: This indicates that processing of the matches is complete. It shall not contain a matching Identifier.
    // b) Pending: This indicates that processing of the matches is initiated or continuing. It shall contain a matching Identifier.
    // c) Refused: Out of Resources: Indicates that processing of the C-FIND has been terminated
    //      because it was out of resources. This may be the initial response to the C-FIND, or may be
    //      sent after a number of pending C-FIND responses. This response shall not contain a matching Identifier.
    // d) Refused: SOP Class Not Supported: Indicates that processing of the C-FIND has been terminated because 
    //      the SOP Class was not supported. This response shall not contain a matching Identifier.
    // e) Cancel: Indicates that the processing of the C-FIND has been terminated due to a C-FIND
    //      Cancel indication primitive. The response shall not contain an Identifier.
    // f) Failed: Indicates that the C-FIND operation failed at the performing DIMSE-service-user.
    enum CFindStatusType { Success,
        Pending,
        RefusedOutOfResources = 0xa701,
        RefusedSOPClassNotSupported,
        IdentifierDoesNotMatchSOPClass = 0xa900,
        Cancel = 0xfe00,
        Failed };

    // C-GET
    // a) Success: This indicates that processing of the matches and all sub-operations are complete.
    // b) Pending: This indicates that processing of the matches and sub-operations is initiated or continuing.
    // c) Refused: Out of Resources: Indicates that processing of the C-GET has been terminated because it was out of resources.
    //      This may be the initial response to the C-GET or may be sent after a number of Pending statuses.
    // d) Refused: SOP Class Not Supported: Indicates that processing of the C-GET has been terminated because the SOP Class was not supported.
    // e) Cancel: Indicates that processing of the C-GET has been terminated due to a C-GET Cancel indication primitive.
    // f) Failed: Indicates that the C-GET operation failed at the performing DIMSE-service-user.
    enum CGetStatusType { Success, Pending, RefusedOutOfResources, RefusedSOPClassNotSupported, Cancel, Failed };

    // C-MOVE
    // a) Success: This indicates that processing of the matches and all sub-operations are complete.
    // b) Pending: This indicates that processing of the matches and sub-operations is initiated or continuing.
    // c) Refused: Out of Resources: Indicates that processing of the C-MOVE has been terminated because it was out of resources.
    //      This may be the initial response to the C-MOVE or may be sent after a number of Pending statuses.
    // d) Refused: SOP Class Not Supported: Indicates that processing of the C-MOVE has been terminated because the SOP Class was not supported.
    // e) Refused: Move Destination Unknown: Indicates that processing of the C-MOVE has been terminated because the Move destination was unknown.
    // f) Cancel: Indicates that processing of the C-MOVE has been terminated due to a C-MOVE Cancel indication primitive.
    // g) Failed: Indicates that the C-MOVE operation failed at the performing DIMSE-service-user.
    enum CMoveStatusType { Success, Pending, RefusedOutOfResources, RefusedSOPClassNotSupported, Cancel, Failed };
*/
