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

#include "wadostorestudyoperationresult.h"

#include "series.h"
#include "study.h"
#include "wadostorerequest.h"

namespace udg {

WadoStoreStudyOperationResult::WadoStoreStudyOperationResult(WadoStoreRequest *request, QObject *parent)
    : StudyOperationResult(parent), m_request(request)
{
    m_requestPacsDevice = request->getPacsDevice();
    m_requestLevel = RequestLevel::Studies;
    m_requestStudy = request->getSeriesList().first()->getParentStudy();
    m_requestStudyInstanceUid = m_requestStudy->getInstanceUID();

    connect(m_request, &WadoRequest::started, this, &WadoStoreStudyOperationResult::onRequestStarted, Qt::DirectConnection);
    connect(m_request, &WadoRequest::finished, this, &WadoStoreStudyOperationResult::onRequestFinished, Qt::DirectConnection);
    connect(m_request, &WadoRequest::cancelled, this, &WadoStoreStudyOperationResult::onRequestCancelled, Qt::DirectConnection);
    connect(m_request, &WadoStoreRequest::instanceSent, [this](int totalInstancesSent) {
        emit instanceTransferred(this, totalInstancesSent);
    });
    connect(m_request, &WadoStoreRequest::seriesSent, [this](int totalSeriesSent) {
        emit seriesTransferred(this, totalSeriesSent);
    });
}

WadoStoreStudyOperationResult::~WadoStoreStudyOperationResult()
{
    delete m_request;
}

StudyOperationResult::OperationType WadoStoreStudyOperationResult::getOperationType() const
{
    return OperationType::Store;
}

void WadoStoreStudyOperationResult::cancel()
{
    m_request->cancel();
}

void WadoStoreStudyOperationResult::onRequestStarted()
{
    emit started(this);
}

void WadoStoreStudyOperationResult::onRequestFinished()
{
    if (m_request->getStatus() == WadoRequest::Status::Clean || m_request->getStatus() == WadoRequest::Status::Warnings)
    {
        setNothing(m_request->getErrorsDescription());
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

void WadoStoreStudyOperationResult::onRequestCancelled()
{
    setCancelled();
}

} // namespace udg
