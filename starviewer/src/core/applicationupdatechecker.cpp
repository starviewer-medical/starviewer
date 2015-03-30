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

#include "applicationupdatechecker.h"
#include "starviewerapplication.h"
#include "logging.h"
#include "machineidentifier.h"
#include "systeminformation.h"
#include "coresettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkInterface>
#include <QNetworkProxyQuery>
#include <QNetworkProxyFactory>
#include <QTimer>

namespace udg {

ApplicationUpdateChecker::ApplicationUpdateChecker(QObject *parent)
: QObject(parent)
{
    m_manager = NULL;

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(15000);

    m_isChecking = false;
}

ApplicationUpdateChecker::~ApplicationUpdateChecker()
{
}

void ApplicationUpdateChecker::checkForUpdates()
{
    if (m_isChecking)
    {
        return;
    }
    m_isChecking = true;

    m_checkedVersion = QString("");
    m_releaseNotesURL = QString("");
    m_updateAvailable = false;
    m_checkOk = true;

    QUrl url(createWebServiceUrl());
    m_manager = new QNetworkAccessManager(this);

    setProxy(url);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkForUpdatesReply(QNetworkReply*)));
    connect(m_timeoutTimer, SIGNAL(timeout()), this, SLOT(checkForUpdatesReplyTimeout()), Qt::DirectConnection);
    
    // Fer la petició
    performOnlinePetition(url);
}

QString ApplicationUpdateChecker::getReleaseNotesUrl() const
{
    return m_releaseNotesURL;
}

QString ApplicationUpdateChecker::getVersion() const
{
    return m_checkedVersion;
}

bool ApplicationUpdateChecker::isNewVersionAvailable() const
{
    return m_updateAvailable;
}

bool ApplicationUpdateChecker::isOnlineCheckOk() const
{
    return m_checkOk;
}

QString ApplicationUpdateChecker::getErrorDescription() const
{
    return m_errorDescription;
}

void ApplicationUpdateChecker::setTimeout(int milliseconds)
{
    m_timeoutTimer->setInterval(milliseconds);
}

int ApplicationUpdateChecker::getTimout() const
{
    return m_timeoutTimer->interval();
}

bool ApplicationUpdateChecker::isChecking() const
{
    return m_isChecking;
}

QString ApplicationUpdateChecker::createWebServiceUrl()
{
    SystemInformation *systemInformation = SystemInformation::newInstance();
    QString operatingSystem = systemInformation->getOperatingSystemAsShortString();
    delete systemInformation;

    QString url = QString("http://starviewer.udg.edu/checknewversion/?currentVersion=%1&os=%2").arg(StarviewerVersionString).arg(operatingSystem);

    QString additionalParametersString = Settings().getValue(CoreSettings::UpdateCheckUrlAdditionalParameters).toString();
    QStringList additionalParameters = additionalParametersString.split(",", QString::SkipEmptyParts);

    foreach (const QString &parameter, additionalParameters)
    {
        QString trimmedParameter = parameter.trimmed();

        if (trimmedParameter == "machineID")
        {
            url += "&machineID=" + MachineIdentifier().getMachineID();
        }
        else if (trimmedParameter == "groupID")
        {
            url += "&groupID=" + MachineIdentifier().getGroupID();
        }
        else
        {
            url += "&" + trimmedParameter;
        }
    }

    return url;
}

void ApplicationUpdateChecker::setProxy(const QUrl &url)
{
    QNetworkProxyQuery q(url);

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(q);
    if (proxies.size() > 0 && proxies[0].type() != QNetworkProxy::NoProxy)
    {
        m_manager->setProxy(proxies[0]);
    }
    QNetworkProxyFactory::setUseSystemConfiguration(false);
}

void ApplicationUpdateChecker::setCheckFinished()
{
    m_isChecking = false;
    m_checkFinished = true;
    emit checkFinished();
}

void ApplicationUpdateChecker::parseWebServiceReply(QNetworkReply *reply)
{
    m_checkOk = false;
    if (reply->error() == QNetworkReply::NoError)
    {
        // Punt d'entrada per el unit testing (readReplyData)
        parseJSON(readReplyData(reply));
    }
    else
    {
        ERROR_LOG(QString("Error buscant noves versions al servidor. La resposta del webservice és del tipus ") +
                  QString::number(reply->error()) + QString(": ") + reply->errorString());
        m_errorDescription = tr("Error connecting to the server. Server response is: %1").arg(reply->errorString());
    }
}

void ApplicationUpdateChecker::parseJSON(const QString &json)
{
    QJsonParseError parseError;
    QJsonObject result = QJsonDocument::fromJson(json.toUtf8(), &parseError).object();

    if (result.value("error").isObject())
    {
        QJsonObject errorObject = result.value("error").toObject();
        ERROR_LOG(QString("Error llegint la resposta del servidor (error en el json) ") + 
                  errorObject.value("code").toString() +
                  QString(": ") + errorObject.value("message").toString());
        m_errorDescription = tr("Error parsing JSON.");
    }
    else
    {
        if (result.value("updateAvailable").isBool())
        {
            m_updateAvailable = result.value("updateAvailable").toBool();
            m_checkOk = true;
            if (m_updateAvailable == true)
            {
                if (result.value("version").isString() && result.value("releaseNotesURL").isString())
                {
                    m_checkedVersion = result.value("version").toString();
                    m_releaseNotesURL = result.value("releaseNotesURL").toString();

                    INFO_LOG(QString("S'ha trobat una nova versió en el servidor, %1.").arg(m_checkedVersion));
                }
                else
                {
                    m_checkOk = false;
                    m_errorDescription = tr("Error parsing JSON: version or releaseNotesURL are not strings");
                }
            }
            else
            {
                INFO_LOG("Starviewer està actualitzat. No s'ha trobat cap versió nova al servidor.");
            }
        }
        else
        {
            // ERROR
            m_errorDescription = tr("Error parsing JSON.");
        }
    }
}

void ApplicationUpdateChecker::performOnlinePetition(const QUrl &url)
{
    m_manager->get(QNetworkRequest(url));
    m_timeoutTimer->start();
}

QString ApplicationUpdateChecker::readReplyData(QNetworkReply *reply)
{
    return QString(reply->readAll());
}

void ApplicationUpdateChecker::checkForUpdatesReply(QNetworkReply *reply)
{
    // Desconectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkForUpdatesReply(QNetworkReply*)));
    m_timeoutTimer->stop();

    // Comprovar si hi ha error a la resposta i si no n'hi ha, es parseja el JSON i es guarda en els atributs de l'objecte.
    // m_checkOk ens dirà si ha anat bé.
    parseWebServiceReply(reply);

    reply->deleteLater();
    m_manager->deleteLater();
    
    setCheckFinished();
}

void ApplicationUpdateChecker::checkForUpdatesReplyTimeout()
{
    // Desconectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkForUpdatesReply(QNetworkReply*)));
   
    m_checkOk = false;
    m_errorDescription = tr("Error requesting release notes: timeout");
    ERROR_LOG("Error en la petició de les release notes. El servidor no respon: Timeout");
    
    delete m_manager;

    setCheckFinished();
}

} // End namespace udg
