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

#include "wadouristudyoperationresult.h"

#include "wadourirequest.h"

namespace udg {

WadoUriStudyOperationResult:: WadoUriStudyOperationResult(WadoUriRequest *request, QObject *parent)
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

    connect(m_request, &WadoRequest::started, this, &WadoUriStudyOperationResult::onRequestStarted, Qt::DirectConnection);
    connect(m_request, &WadoRequest::finished, this, &WadoUriStudyOperationResult::onRequestFinished, Qt::DirectConnection);
    connect(m_request, &WadoRequest::cancelled, this, &WadoUriStudyOperationResult::onRequestCancelled, Qt::DirectConnection);
    connect(m_request, &WadoUriRequest::instanceDownloaded, [this](int totalInstancesDownloaded) {
        emit instanceTransferred(this, totalInstancesDownloaded);
    });
    connect(m_request, &WadoUriRequest::seriesDownloaded, [this](int totalSeriesDownloaded) {
        emit seriesTransferred(this, totalSeriesDownloaded);
    });
}

WadoUriStudyOperationResult::~WadoUriStudyOperationResult()
{
    delete m_request;
}

StudyOperationResult::OperationType WadoUriStudyOperationResult::getOperationType() const
{
    return OperationType::Retrieve;
}

void WadoUriStudyOperationResult::setRequestStudy(const Study *requestStudy)
{
    m_requestStudy = requestStudy;
}

void WadoUriStudyOperationResult::cancel()
{
    m_request->cancel();
}

void WadoUriStudyOperationResult::onRequestStarted()
{
    emit started(this);
}

void WadoUriStudyOperationResult::onRequestFinished()
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

void WadoUriStudyOperationResult::onRequestCancelled()
{
    setCancelled();
}

} // namespace udg
