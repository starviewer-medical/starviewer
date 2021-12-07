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

#include "wadosearchstudyoperationresult.h"

#include "studyoperations.h"
#include "wadosearchrequest.h"

namespace udg {

WadoSearchStudyOperationResult::WadoSearchStudyOperationResult(WadoSearchRequest *request, QObject *parent)
    : StudyOperationResult(parent), m_request(request)
{
    m_requestPacsDevice = request->getPacsDevice();
    m_requestStudyInstanceUid = request->getDicomMask().getStudyInstanceUID();
    m_requestSeriesInstanceUid = request->getDicomMask().getSeriesInstanceUID();
    m_requestSopInstanceUid = request->getDicomMask().getSOPInstanceUID();

    switch (m_request->getTargetResource())
    {
        case StudyOperations::TargetResource::Studies:
            m_requestLevel = RequestLevel::Studies;
            break;
        case StudyOperations::TargetResource::Series:
            m_requestLevel = RequestLevel::Series;
            break;
        case StudyOperations::TargetResource::Instances:
            m_requestLevel = RequestLevel::Instances;
            break;
    }

    connect(m_request, &WadoRequest::started, this, &WadoSearchStudyOperationResult::onRequestStarted, Qt::DirectConnection);
    connect(m_request, &WadoRequest::finished, this, &WadoSearchStudyOperationResult::onRequestFinished, Qt::DirectConnection);
    connect(m_request, &WadoRequest::cancelled, this, &WadoSearchStudyOperationResult::onRequestCancelled, Qt::DirectConnection);
}

WadoSearchStudyOperationResult::~WadoSearchStudyOperationResult()
{
    delete m_request;
}

StudyOperationResult::OperationType WadoSearchStudyOperationResult::getOperationType() const
{
    return OperationType::Search;
}

void WadoSearchStudyOperationResult::cancel()
{
    m_request->cancel();
}

void WadoSearchStudyOperationResult::onRequestStarted()
{
    emit started(this);
}

void WadoSearchStudyOperationResult::onRequestFinished()
{
    if (m_request->getStatus() == WadoRequest::Status::Clean || m_request->getStatus() == WadoRequest::Status::Warnings)
    {
        switch (m_requestLevel)
        {
            case RequestLevel::Studies:
                setStudies(m_request->getStudies(), m_request->getErrorsDescription());
                break;
            case RequestLevel::Series:
                setSeries(m_request->getSeries(), m_request->getErrorsDescription());
                break;
            case RequestLevel::Instances:
                setInstances(m_request->getInstances(), m_request->getErrorsDescription());
                break;
        }
    }
    else if (m_request->getStatus() == WadoRequest::Status::Errored)
    {
        setErrorText(m_request->getErrorsDescription());
    }
    else
    {
        onRequestCancelled();
    }
}

void WadoSearchStudyOperationResult::onRequestCancelled()
{
    setCancelled();
}

} // namespace udg
