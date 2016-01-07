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

#include "dicomserviceresponsestatus.h"

#include "dicomdictionary.h"
#include "logging.h"

namespace udg {

const unsigned int DICOMServiceResponseStatus::RefusedOutOfResources = 0xa700;
const unsigned int DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToCalculateNumberOfMatches = 0xa701;
const unsigned int DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToPerformSubOperations = 0xa702;
const unsigned int DICOMServiceResponseStatus::RefusedMoveDestinationUnknown = 0xa801;
const unsigned int DICOMServiceResponseStatus::IdentifierDoesNotMatchSOPClass = 0xa900;
const unsigned int DICOMServiceResponseStatus::ErrorDataSetDoesNotMatchSOPClass = 0xa900;
const unsigned int DICOMServiceResponseStatus::ErrorCannotUnderstand = 0xc000;
const unsigned int DICOMServiceResponseStatus::UnableToProcess = 0xc000;
const unsigned int DICOMServiceResponseStatus::TerminatedDueToCancelRequest = 0xfe00;
const unsigned int DICOMServiceResponseStatus::WarningSubOperationsCompleteOneOrMoreFailures = 0xb000;
const unsigned int DICOMServiceResponseStatus::WarningCoercionOfDataElements = 0xb000;
const unsigned int DICOMServiceResponseStatus::WarningDatasetDoesNotMatchSOPClass = 0xb007;
const unsigned int DICOMServiceResponseStatus::WarningElementsDiscarded = 0xb006;
const unsigned int DICOMServiceResponseStatus::Pending = 0xff00;
const unsigned int DICOMServiceResponseStatus::PendingWarningUnsupportedOptionalKeys = 0xff01;
const unsigned int DICOMServiceResponseStatus::Success = 0x0000;

DICOMServiceResponseStatus::DICOMServiceResponseStatus()
{
    m_statusCode = Success;
    m_serviceType = UnknownService;
}

DICOMServiceResponseStatus::~DICOMServiceResponseStatus()
{
}

void DICOMServiceResponseStatus::setStatusCode(unsigned int code)
{
    m_statusCode = code;
}

unsigned int DICOMServiceResponseStatus::getStatusCode() const
{
    return m_statusCode;
}

void DICOMServiceResponseStatus::setServiceType(ServiceType service)
{
    m_serviceType = service;
}

DICOMServiceResponseStatus::ServiceType DICOMServiceResponseStatus::getServiceType() const
{
    return m_serviceType;
}

QString DICOMServiceResponseStatus::getStatusCodeAsString() const
{
    // dcmtk's dimse.h functions (DU_cmoveStatusString, DU_cfindStatusString, DU_storeStatusString,...) could be also used
    // to translate codes into strings, but we'll loss the possibility of translate them
    switch (m_statusCode)
    {
        case RefusedOutOfResources:
            return QObject::tr("Refused: Out of resources");
        
        case RefusedOutOfResourcesUnableToCalculateNumberOfMatches:
            return QObject::tr("Refused: Out of resources - Unable to calculate number of matches");

        case RefusedOutOfResourcesUnableToPerformSubOperations:
            return QObject::tr("Refused: Out of resources - Unable to perform sub-operations");

        case RefusedMoveDestinationUnknown:
            return QObject::tr("Refused: Move destination unknown");

        case IdentifierDoesNotMatchSOPClass:
            if (m_serviceType == CStore)
            {
                // ErrorDataSetDoesNotMatchSOPClass
                return QObject::tr("Error: Data set does not match SOP Class");
            }
            else
            {
                return QObject::tr("Identifier does not match SOP Class");
            }

        case UnableToProcess:
            if (m_serviceType == CStore)
            {
                // ErrorCannotUnderstand
                return QObject::tr("Error: Cannot understand");
            }
            else
            {
                return QObject::tr("Unable to process");
            }

        case TerminatedDueToCancelRequest:
            return QObject::tr("Terminated due to cancel request");

        case WarningSubOperationsCompleteOneOrMoreFailures:
            if (m_serviceType == CStore)
            {
                // WarningCoercionOfDataElements
                return QObject::tr("Warning: Coercion of data elements");
            }
            else
            {
                return QObject::tr("Warning: Sub-operations complete, one or more failures");
            }

        case WarningDatasetDoesNotMatchSOPClass:
            return QObject::tr("Warning: Data set does not match SOP Class");

        case WarningElementsDiscarded:
            return QObject::tr("Warning: Elements discarded");
        
        case Pending:
            return QObject::tr("Pending");
        
        case PendingWarningUnsupportedOptionalKeys:
            return QObject::tr("Pending: Warning - One or more optional keys were not supported");

        case Success:
            return QObject::tr("Success");
        
        default:
            return QObject::tr("Unknown status");
    }
}

DICOMServiceResponseStatus::ServiceStatusType DICOMServiceResponseStatus::getServiceStatus() const
{
    switch (m_statusCode)
    {
        case RefusedOutOfResources:
        case RefusedOutOfResourcesUnableToCalculateNumberOfMatches:
        case RefusedOutOfResourcesUnableToPerformSubOperations:
        case RefusedMoveDestinationUnknown:
        case IdentifierDoesNotMatchSOPClass:
        case UnableToProcess:
            return FailureStatus;
            break;

        case TerminatedDueToCancelRequest:
            return CancelStatus;
            break;

        case WarningSubOperationsCompleteOneOrMoreFailures:
        case WarningDatasetDoesNotMatchSOPClass:
        case WarningElementsDiscarded:
            return WarningStatus;
            break;

        case Pending:
        case PendingWarningUnsupportedOptionalKeys:
            return PendingStatus;
            break;

        case Success:
            return SuccessStatus;
            break;

        default:
            return UnknownStatus;
            break;
    }
}

QList<DICOMTag> DICOMServiceResponseStatus::getRelatedFields()
{
    QList<DICOMTag> relatedFields;

    switch (m_statusCode)
    {
        case RefusedOutOfResources:
        case RefusedOutOfResourcesUnableToCalculateNumberOfMatches:
        case RefusedMoveDestinationUnknown:
            relatedFields << DICOMErrorComment;
            break;

        case RefusedOutOfResourcesUnableToPerformSubOperations:
        case WarningSubOperationsCompleteOneOrMoreFailures:
        case Success:
            relatedFields << DICOMNumberOfCompletedSubOperations << DICOMNumberOfFailedSubOperations << DICOMNumberOfWarningSubOperations;
            break;

        case IdentifierDoesNotMatchSOPClass:
        case UnableToProcess:
        case WarningDatasetDoesNotMatchSOPClass:
        case WarningElementsDiscarded:
            relatedFields << DICOMOffendingElement << DICOMErrorComment;
            break;

        case TerminatedDueToCancelRequest:
        case Pending:
            relatedFields << DICOMNumberOfRemainingSubOperations << DICOMNumberOfCompletedSubOperations << DICOMNumberOfFailedSubOperations 
                << DICOMNumberOfWarningSubOperations;
            break;
    }

    return relatedFields;
}

void DICOMServiceResponseStatus::addRelatedFieldValue(DICOMValueAttribute *value)
{
    if (value)
    {
        m_relatedFieldsValues << value;
    }
}

QString DICOMServiceResponseStatus::toString() const
{
    QString result;

    result = QObject::tr("DIMSE Status: ") + getStatusCodeAsString();
    foreach (DICOMValueAttribute *value, m_relatedFieldsValues)
    {
        result += "\n" + value->toString(true);
    }

    return result;
}

void DICOMServiceResponseStatus::clear()
{
    m_statusCode = Success;
    
    foreach (DICOMValueAttribute *value, m_relatedFieldsValues)
    {
        delete value;
    }
    m_relatedFieldsValues.clear();
}

void DICOMServiceResponseStatus::dumpLog()
{
    switch (getServiceStatus())
    {
        case DICOMServiceResponseStatus::FailureStatus:
        case DICOMServiceResponseStatus::UnknownStatus:
            ERROR_LOG(getServiceTypeAsString() + " operation has failed and could not be performed. Error: " + getStatusCodeAsString());
            break;

        case DICOMServiceResponseStatus::WarningStatus:
            WARN_LOG(getServiceTypeAsString() + " operation has failed partially and could not be performed completely. Error: " + getStatusCodeAsString());
            break;

        default:
            break;
    }

    if (getServiceStatus() != DICOMServiceResponseStatus::SuccessStatus)
    {
        INFO_LOG("Response status details");
        INFO_LOG(toString());
    }
}

QString DICOMServiceResponseStatus::getServiceTypeAsString() const
{
    QString serviceType;

    switch (m_serviceType)
    {
        case CFind:
            serviceType = "C-FIND (Query)";
            break;

        case CMove:
            serviceType = "C-MOVE (Retrieve)";
            break;

        case CStore:
            serviceType = "C-STORE (Send)";
            break;

        case CGet:
            serviceType = "C-GET (Query)";
            break;

        case UnknownService:
            serviceType = "Unknown service";
            break;
    }

    return serviceType;
}

} // end namespace udg
