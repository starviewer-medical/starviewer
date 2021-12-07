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

#include "wadorequestmanager.h"

#include "wadorequest.h"

#include <QNetworkAccessManager>

namespace udg {

WadoRequestManager::WadoRequestManager(QObject *parent)
    : QObject(parent), m_networkAccessManager(nullptr)
{
    connect(this, &WadoRequestManager::startCalled, this, &WadoRequestManager::startInternal);
}

void WadoRequestManager::start(WadoRequest *request)
{
    emit startCalled(request);
}

void WadoRequestManager::startInternal(WadoRequest *request)
{
    if (!m_networkAccessManager)
    {
        // Instantiated here so that it lives in the WADO thread
        m_networkAccessManager = new QNetworkAccessManager(this);
    }

    request->start(m_networkAccessManager);
}

} // namespace udg
