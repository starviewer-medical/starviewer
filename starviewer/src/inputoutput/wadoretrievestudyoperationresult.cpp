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

#include "wadoretrievestudyoperationresult.h"

#include "wadoretrieverequest.h"

namespace udg {

WadoRetrieveStudyOperationResult::WadoRetrieveStudyOperationResult(WadoRetrieveRequest *request, QObject *parent)
    : StudyOperationResult(parent), m_request(request)
{
    m_requestPacsDevice = request->getPacsDevice();
    m_requestStudyInstanceUid = request->getStudyInstanceUid();
    m_requestSeriesInstanceUid = request->getSeriesInstanceUid();
    m_requestSopInstanceUid = request->getSopInstanceUid();

    if (!m_requestSopInstanceUid.isEmpty())
    {
        m_requestLevel = RequestLevel::Instances;
    }
    else if (!m_requestSeriesInstanceUid.isEmpty())
    {
        m_requestLevel = RequestLevel::Series;
    }
    else
    {
        m_requestLevel = RequestLevel::Studies;
    }

    connect(m_request, &WadoRequest::started, this, &WadoRetrieveStudyOperationResult::onRequestStarted, Qt::DirectConnection);
    connect(m_request, &WadoRequest::finished, this, &WadoRetrieveStudyOperationResult::onRequestFinished, Qt::DirectConnection);
    connect(m_request, &WadoRequest::cancelled, this, &WadoRetrieveStudyOperationResult::onRequestCancelled, Qt::DirectConnection);
    connect(m_request, &WadoRetrieveRequest::instanceDownloaded, [this](int totalInstancesDownloaded) {
        emit instanceTransferred(this, totalInstancesDownloaded);
    });
    connect(m_request, &WadoRetrieveRequest::seriesDownloaded, [this](int totalSeriesDownloaded) {
        emit seriesTransferred(this, totalSeriesDownloaded);
    });
}

WadoRetrieveStudyOperationResult::~WadoRetrieveStudyOperationResult()
{
}

StudyOperationResult::OperationType WadoRetrieveStudyOperationResult::getOperationType() const
{
    return OperationType::Retrieve;
}

void WadoRetrieveStudyOperationResult::setRequestStudy(const Study *requestStudy)
{
    m_requestStudy = requestStudy;
}

void WadoRetrieveStudyOperationResult::cancel()
{
    m_request->cancel();
}

void WadoRetrieveStudyOperationResult::onRequestStarted()
{
    emit started(this);
}

void WadoRetrieveStudyOperationResult::onRequestFinished()
{
    if (m_request->getStatus() == WadoRequest::Status::Clean || m_request->getStatus() == WadoRequest::Status::Warnings)
    {
        setStudyInstanceUid(m_request->getStudyInstanceUid(), m_request->getErrorsDescription());
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

void WadoRetrieveStudyOperationResult::onRequestCancelled()
{
    setCancelled();
}

} // namespace udg
