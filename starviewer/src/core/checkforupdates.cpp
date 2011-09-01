#include "checkforupdates.h"
#include "starviewerapplication.h"
#include "logging.h"
#include "machineidentifier.h"

#include <QScriptEngine>
#include <QScriptValue>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkInterface>
#include <QNetworkProxyQuery>
#include <QNetworkProxyFactory>

namespace udg {

CheckForUpdates::CheckForUpdates(QObject *parent)
: QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

CheckForUpdates::~CheckForUpdates()
{
}

void CheckForUpdates::checkForUpdates()
{
    m_checkedVersion = QString("");
    m_releaseNotesURL = QString("");
    m_updateAvailable = false;
    m_checkOk = true;

    QUrl url(createWebServiceUrl());
    setProxy(url);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(checkForUpdatesReply(QNetworkReply *)));
    m_manager->get(QNetworkRequest(url));
}

QString CheckForUpdates::getReleaseNotesUrl() const
{
    return m_releaseNotesURL;
}

QString CheckForUpdates::getVersion() const
{
    return m_checkedVersion;
}

bool CheckForUpdates::isNewVersionAvailable() const
{
    return m_updateAvailable;
}

bool CheckForUpdates::isOnlineCheckOk() const
{
    return m_checkOk;
}

QString CheckForUpdates::getErrorDescription() const
{
    return m_errorDescription;
}

QString CheckForUpdates::createWebServiceUrl()
{
    MachineIdentifier machineIdentifier;

    QString machineID = machineIdentifier.getMachineID();
    QString groupID = machineIdentifier.getGroupID();

    return QString("http://starviewer.udg.edu/checknewversion/?currentVersion=%1&machineID=%2&groupID=%3")
              .arg(StarviewerVersionString).arg(machineID).arg(groupID);
}

void CheckForUpdates::setProxy(const QUrl &url)
{
    QNetworkProxyQuery q(url);

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(q);
    if (proxies.size() > 0 && proxies[0].type() != QNetworkProxy::NoProxy)
    {
        m_manager->setProxy(proxies[0]);
    }
}

void CheckForUpdates::setCheckFinished()
{
    m_checkFinished = true;
    emit checkFinished();
}

void CheckForUpdates::parseWebServiceReply(QNetworkReply *reply)
{
    m_checkOk = false;
    if (reply->error() == QNetworkReply::NoError)
    {
        parseJSON(reply->readAll());
    }
    else
    {
        ERROR_LOG(QString("Error buscant noves versions al servidor. La resposta del webservice és del tipus ") +
                  QString::number(reply->error()) + QString(": ") + reply->errorString());
        m_errorDescription = QString(tr("Error connecting to the server. Server response is: %1"))
                                .arg(reply->errorString());
    }
}

void CheckForUpdates::parseJSON(const QString &json)
{
    QScriptValue scriptValue;
    QScriptEngine engine;
    scriptValue = engine.evaluate("(" + json + ")");

    if (scriptValue.property("error").isObject())
    {
        ERROR_LOG(QString("Error llegint la resposta del servidor (error en el json) ") + 
                  scriptValue.property("error").property("code").toString() +
                  QString(": ") + scriptValue.property("error").property("message").toString());
        m_errorDescription = tr("Error parsing json.");
    }
    else
    {
        if (scriptValue.property("updateAvailable").isBool())
        {
            m_updateAvailable = scriptValue.property("updateAvailable").toBool();
            m_checkOk = true;
            if (m_updateAvailable == true)
            {
                if (scriptValue.property("version").isString() && scriptValue.property("releaseNotesURL").isString())
                {
                    m_checkedVersion = scriptValue.property("version").toString();
                    m_releaseNotesURL = scriptValue.property("releaseNotesURL").toString();

                    INFO_LOG(QString("S'ha trobat una nova versió en el servidor, %1.").arg(m_checkedVersion));
                }
                else
                {
                    m_checkOk = false;
                    m_errorDescription = tr("Error parsing json. Version or releaseNotesUrl is not a String");
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
            m_errorDescription = tr("Error parsing json.");
        }
    }
}

void CheckForUpdates::checkForUpdatesReply(QNetworkReply *reply)
{
    // Desconnectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(checkForUpdatesReply(QNetworkReply *)));

    // Comprovar si hi ha error a la resposta i si no n'hi ha, es parseja el JSON i es guarda en els atributs de l'objecte.
    // m_checkOk ens dirà si ha anat bé.
    parseWebServiceReply(reply);

    if (m_checkOk && m_updateAvailable)
    {
        connect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(doesUpdateNotesUrlExistOnServerReply(QNetworkReply *)));
        m_manager->get(QNetworkRequest(QUrl(m_releaseNotesURL)));
    }
    else
    {
        setCheckFinished();
    }

    reply->deleteLater();
}

void CheckForUpdates::doesUpdateNotesUrlExistOnServerReply(QNetworkReply *reply)
{
    
    // Desconnectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(doesUpdateNotesUrlExistOnServerReply(QNetworkReply *)));

    bool error = reply->error() != QNetworkReply::NoError;
    if (error)
    {
        ERROR_LOG(QString("Error en rebre les notes de la versio %1, tipus ").arg(m_checkedVersion) + QString::number(reply->error())+
                  QString(": ") + reply->errorString());

        m_checkOk = false;
        m_errorDescription = QString(tr("The requested release notes does not exist on the server. Server response is: %1"))
                                .arg(reply->errorString());
    }
    reply->deleteLater();
    setCheckFinished();
}

} // End namespace udg
