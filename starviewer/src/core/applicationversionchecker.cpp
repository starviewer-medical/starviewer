/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "applicationversionchecker.h"
#include "coresettings.h"
#include "starviewerapplication.h"
#include "logging.h"

#include <QScriptEngine>
#include <QScriptValue>
#include <QUrl>
#include <QFile>
#include <QDate>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QNetworkProxyQuery>
#include <QNetworkProxyFactory>
#include <QProcessEnvironment>

namespace udg {
    class QReleaseNotes;

ApplicationVersionChecker::ApplicationVersionChecker()
{
    //per defecte diem que volem comprobar la nova versió, si no es canvia
    m_checkNewVersion = true;
    // la versio que ens retorna el servidor per defecte es cadena buida
    m_checkedVersion = QString("");
    //inicialitzem a null m_releaseNotes i m_manager
    m_manager = NULL;
    m_releaseNotes = NULL;
}

ApplicationVersionChecker::~ApplicationVersionChecker()
{
    // destruir el manager de connexions
    if (m_manager)
    {
        delete m_manager;
    }
     // i la finestra de les release notes
    if (m_releaseNotes)
    {
        delete m_releaseNotes;
    }
}

void ApplicationVersionChecker::checkReleaseNotes()
{
    m_checkNewVersion = true;
    m_checkFinished = false;
    m_somethingToShow = false;
    bool async = false;

    //utilitzar els settings
    readSettings();

    if (m_showReleaseNotesFirstTime && !m_neverShowReleaseNotes)
    {
        //llegir el contingut dels fitxers HTML de les release notes
        QUrl url = createLocalUrl();
        if (checkLocalUrl(url))
        {
            m_checkNewVersion = false;
            m_releaseNotes = new QReleaseNotes(0);
            m_releaseNotes->setUrl(url);
            m_releaseNotes->setWindowTitle(tr("Release Notes"));
            m_somethingToShow = true;
            
            connect(m_releaseNotes, SIGNAL(closing()), this, SLOT(closeEvent()));
            setCheckFinished();
        }
    }
    else if (m_checkNewVersion)
    {
        if (checkTimeInterval())
        {
            m_releaseNotes = new QReleaseNotes(0);
            m_releaseNotes->setDontShowVisible(false);
            m_releaseNotes->setWindowTitle(tr("New Version Available"));
            connect(m_releaseNotes, SIGNAL(closing()), this, SLOT(closeEvent()));
            checkVersionOnServer();
            async = true;
        }
    }
    if (!async)
    {
        setCheckFinished();
    }
}

void ApplicationVersionChecker::showIfCorrect()
{
    if (m_checkFinished)
    {
        // Aqui basicament hi entra quan mostra les notes locals
        if (m_somethingToShow)
        {
            m_releaseNotes->show();
        }
    }
    else
    {
        // Basicament hi entra quan mostra les notes d'una nova versió al server
        connect(this, SIGNAL(checkFinished()), this, SLOT(showWhenCheckFinished()));
    }
}

void ApplicationVersionChecker::setCheckVersionInterval(int interval)
{
    if (interval > 0)
    {
        Settings settings;
        settings.setValue(CoreSettings::CheckVersionInterval, interval);
    }
}

QUrl ApplicationVersionChecker::createLocalUrl()
{
    //agafar el path del fitxer
    QString defaultPath = qApp->applicationDirPath() + "/releasenotes/";
    if (!QFile::exists(defaultPath))
    {
        /// Mode desenvolupament
        defaultPath = qApp->applicationDirPath() + "/../releasenotes/";
    }

    QStringList versionList = udg::StarviewerVersionString.split(QString("."));
    QString version(versionList[0] + "." + versionList[1] + "." + versionList[2]);
    QUrl result = defaultPath + "releasenotes" + version + ".html";
    return result;
}

bool ApplicationVersionChecker::checkLocalUrl(QUrl url){
    //comprobar si existeix localment el fitxer
    return QFile::exists(QString(url.toString()));
}

bool ApplicationVersionChecker::checkTimeInterval()
{
    // s'utilitzen dues dates, l'actual i la última comprobada
    QDate today = QDate::currentDate();
    QDate lastTime = QDate::fromString(m_lastVersionCheckedDate, QString("yyyyMMdd"));
    
    int i = m_checkVersionInterval;

    //si la data que hi ha guardada a les settings és correcte
    if (lastTime.isValid())
    {
        // calcular quans dies han passat des de que es va comprobar
        i = lastTime.daysTo(today);
    }
    //si la data no es correcte llavors i = interval i es tornarà a comprobar
        
    // retornar si fa tants dies o més com marca l'interval que no s'ha comprobat
    return i >= m_checkVersionInterval;
}

void ApplicationVersionChecker::checkVersionOnServer()
{
    QUrl url(createWebServiceUrl());
    m_manager = new QNetworkAccessManager(this);
    setProxy(url);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(webServiceReply(QNetworkReply*)));
    m_manager->get(QNetworkRequest(url));
}

QString ApplicationVersionChecker::createWebServiceUrl()
{
    // Buscar l'adreces MAC del host.
    QString macAdress(""); 
    // Buscar el domini
    QString domain("");

    // Primer de tot mirar si hi ha interficia de xarxa local
    bool found = false;
    int index = 0;
    while(!found && index < QNetworkInterface::allInterfaces().count())
    {
        QNetworkInterface inter = QNetworkInterface::allInterfaces()[index++];

        bool isMainInterface = false;
#ifdef WIN32
        isMainInterface = inter.humanReadableName().contains("local");
#else
        isMainInterface = inter.humanReadableName().contains("eth");
#endif
        QNetworkInterface::InterfaceFlags f = inter.flags();
        bool flagsOk = f.testFlag(QNetworkInterface::IsUp) && f.testFlag(QNetworkInterface::IsRunning) && !f.testFlag(QNetworkInterface::IsLoopBack);
        
        if (isMainInterface && inter.isValid() && flagsOk)
        {
            macAdress += inter.hardwareAddress();
            found = true;
        }
    }

    // Dono prioritat a la interfície d'àrea local i després busco la primera interfície vàlida
    if (macAdress == "")
    {
        found = false;
        index = 0;
        while(!found && index < QNetworkInterface::allInterfaces().count())
        {
            QNetworkInterface inter = QNetworkInterface::allInterfaces()[index++];

            QNetworkInterface::InterfaceFlags f = inter.flags();
            bool flagsOk = f.testFlag(QNetworkInterface::IsUp) && f.testFlag(QNetworkInterface::IsRunning) && !f.testFlag(QNetworkInterface::IsLoopBack);

            //Per si de cas el bluetooth està engegat i foncionant, fer que no l'agafi
            //Rarament trobarem una connexió de xarxa que vagi a través d'un dispositiu bluetooth
            if (inter.isValid() && flagsOk && !inter.humanReadableName().toLower().contains("bluetooth"))
            {
                macAdress += inter.hardwareAddress();
                found = true;
            }
        }
    }
    
 // En cas que estem a windows, Busquem el groupID
#ifdef WIN32
    domain = QProcessEnvironment::systemEnvironment().value(QString("USERDOMAIN"),QString(""));
#endif

    QString machineID = encryptBase64Url(macAdress);
    QString groupID = encryptBase64Url(domain);

    QString result;
    QStringList versionList = udg::StarviewerVersionString.split(QString("."));
    QString version(versionList[0] + "." + versionList[1] + "." + versionList[2]);
    result = QString("http://trueta.udg.edu/Fusio/updateavailable/?currentVersion=%1&machineID=%2&groupID=%3").arg(version).arg(machineID).arg(groupID);
    
    return result;
}

QString ApplicationVersionChecker::encryptBase64Url(QString url)
{
    return QString(QCryptographicHash::hash(url.toAscii(), QCryptographicHash::Sha1).toBase64().replace("=","").replace("+","-").replace("/","_"));
}

void ApplicationVersionChecker::setProxy(QUrl url)
{
    QNetworkProxyQuery q(url);

    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(q);
    if (proxies.size() > 0 && proxies[0].type() != QNetworkProxy::NoProxy)
    {
        m_manager->setProxy(proxies[0]);
    }
}

void ApplicationVersionChecker::writeSettings()
{
    Settings settings;
    if (!m_checkNewVersion)
    {
        //ja no es mostren més fins la proxima actualització
        settings.setValue(CoreSettings::ShowReleaseNotesFirstTime, false);

        //primer de tot comprobar si el 'Don't show on future releases' esta activat
        if (m_releaseNotes->isDontShowAnymoreChecked())
        {
            //modificar els settings per que no es mostrin mai més  
            settings.setValue(CoreSettings::NeverShowReleaseNotes, true);
        }
    }
    else
    {
        //Guardar la data en que hem comprobat la versió
        settings.setValue(CoreSettings::LastVersionCheckedDate, QDate::currentDate().toString(QString("yyyyMMdd")));
        //Guardar la versió que hem comprobat
        settings.setValue(CoreSettings::LastVersionChecked, m_checkedVersion);
    }
}

void ApplicationVersionChecker::readSettings()
{
    Settings settings;
    m_showReleaseNotesFirstTime = settings.getValue(CoreSettings::ShowReleaseNotesFirstTime).toBool();
    m_neverShowReleaseNotes = settings.getValue(CoreSettings::NeverShowReleaseNotes).toBool();
    m_lastVersionCheckedDate = settings.getValue(CoreSettings::LastVersionCheckedDate).toString();
    m_lastVersionChecked = settings.getValue(CoreSettings::LastVersionChecked).toString();
    m_checkVersionInterval = settings.getValue(CoreSettings::CheckVersionInterval).toInt();
}

void ApplicationVersionChecker::setCheckFinished()
{
    m_checkFinished = true;
    emit checkFinished();
}

void ApplicationVersionChecker::webServiceReply(QNetworkReply *reply)
{
    //desconnectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(webServiceReply(QNetworkReply*)));

    bool result = reply->error() == QNetworkReply::NoError;
    if (result) // si no error
    {
        QString json(reply->readAll());

        QString version("");
        QString releaseNotesURL("");
        
        QScriptValue scriptValue; 
        QScriptEngine engine;
        scriptValue = engine.evaluate("(" + json + ")");
     
        if (scriptValue.property("error").isObject())
        {
            ERROR_LOG(QString("Error parsejant el json ") + scriptValue.property("error").property("code").toString() + QString(": ") + scriptValue.property("error").property("message").toString());
        }
        else
        {
            if (scriptValue.property("updateAvailable").isBool() && scriptValue.property("updateAvailable").toBool() == true)
            {
                m_checkedVersion = scriptValue.property("version").toString();
                releaseNotesURL = scriptValue.property("releaseNotesURL").toString();
            }
        }

        connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNotesUrlReply(QNetworkReply*)));
        m_manager->get(QNetworkRequest(QUrl(releaseNotesURL)));
    }
    else
    {
        ERROR_LOG(QString("Error de la resposta del webservice, tipus ") + QString::number(reply->error()) + QString(": ") + reply->errorString());
        setCheckFinished();
    }
    reply->deleteLater();
}

void ApplicationVersionChecker::updateNotesUrlReply(QNetworkReply *reply)
{
    //desconnectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNotesUrlReply(QNetworkReply*)));

    bool result = reply->error() == QNetworkReply::NoError;
    if (result) // si no error
    {
        if (m_lastVersionChecked != m_checkedVersion)
        {
            m_releaseNotes->setUrl(reply->url());
            m_somethingToShow = true;
        }
    }
    else
    {
        ERROR_LOG(QString("Error en rebre les notes de la versio, tipus ") + QString::number(reply->error()) + QString(": ") + reply->errorString());        
    }
    reply->deleteLater();
    setCheckFinished();       
}

void ApplicationVersionChecker::closeEvent()
{
    //guardar els settings
    writeSettings();
}

void ApplicationVersionChecker::showWhenCheckFinished()
{
    if (m_somethingToShow)
    {
        m_releaseNotes->show();
    }
}

}; // end namespace udg
