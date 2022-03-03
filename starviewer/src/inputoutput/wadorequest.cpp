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

#include "wadorequest.h"

#include <QNetworkReply>

namespace udg {

WadoRequest::WadoRequest(const PacsDevice &pacs, QObject *parent)
    : QObject(parent), m_networkAccessManager(nullptr), m_reply(nullptr), m_status(Status::Clean), m_pacs(pacs)
{
}

WadoRequest::~WadoRequest()
{
    if (m_reply)
    {
        m_reply->deleteLater();
    }
}

const PacsDevice& WadoRequest::getPacsDevice() const
{
    return m_pacs;
}

void WadoRequest::start(QNetworkAccessManager *networkAccessManager)
{
    Q_ASSERT(networkAccessManager);
    m_networkAccessManager = networkAccessManager;
    startInternal();
}

void WadoRequest::cancel()
{
    if (m_reply)
    {
        m_reply->abort();
    }
}

WadoRequest::Status WadoRequest::getStatus() const
{
    return m_status;
}

const QString& WadoRequest::getErrorsDescription() const
{
    return m_errorsDescription;
}

} // namespace udg
