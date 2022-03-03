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

#ifndef UDG_WADOREQUESTMANAGER_H
#define UDG_WADOREQUESTMANAGER_H

#include <QObject>

class QNetworkAccessManager;

namespace udg {

class WadoRequest;

/**
 * @brief The WadoRequestManager class manages the process of running WadoRequests in the thread where it lives.
 *
 * start() can be called from any thread and then it will asynchronously call the request's start() method in the WADO thread.
 *
 * The QNetworkAccessManager is created in the first call to startInternal() so that it lives in the WADO thread.
 */
class WadoRequestManager : public QObject
{
    Q_OBJECT

public:
    /// Creates an instance of the manager with a null QNetworkAccessManager and creates the internal connections.
    explicit WadoRequestManager(QObject *parent = nullptr);

    /// Asynchronously starts the given request. Emits startCalled.
    void start(WadoRequest *request);

signals:
    /// Internal signal used to call startInternal in the thread where this manager lives.
    void startCalled(WadoRequest *request);

private slots:
    /// Creates a QNetworkAccessManager in the first call and calls start on the request, all from the thread where this manager lives.
    void startInternal(WadoRequest *request);

private:
    /// QNetworkAccessManager instance that will be given to the WadoRequests.
    QNetworkAccessManager *m_networkAccessManager;
};

} // namespace udg

#endif // UDG_WADOREQUESTMANAGER_H
