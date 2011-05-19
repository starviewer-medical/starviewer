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
#include <QStringList>
#include <QMessageBox>

namespace udg {
    class QReleaseNotes;

ApplicationVersionChecker::ApplicationVersionChecker(QObject *parent)
: QObject(parent)
{
    // Per defecte diem que volem comprobar la nova versió, si no es canvia
    m_checkNewVersion = true;
    // La versio que ens retorna el servidor per defecte es cadena buida
    m_checkedVersion = QString("");
    // Inicialitzem a null m_releaseNotes i m_manager
    m_manager = NULL;
    m_releaseNotes = NULL;
}

ApplicationVersionChecker::~ApplicationVersionChecker()
{
    // Destruir el manager de connexions
    delete m_manager;
    // I la finestra de les release notes
    delete m_releaseNotes;
}

void ApplicationVersionChecker::checkReleaseNotes()
{
    m_checkNewVersion = true;
    m_checkFinished = false;
    m_somethingToShow = false;
    bool async = false;

    // Utilitzar els settings
    readSettings();
    // Si no hi ha nova versió al server guardem l'actual als settings
    m_checkedVersion = m_lastVersionChecked;

    if (isDevelopmentMode())
    {
        // En mode desenvolupament no es mostraran les notes ni es faran comprovacions online
        return;
    }

    if (isNewVersionInstalled())
    {
        // Llegir el contingut dels fitxers HTML de les release notes
        QUrl url = createLocalUrl();
        if (checkLocalUrl(url))
        {
            delete m_releaseNotes;
            m_releaseNotes = new QReleaseNotes(0);
            m_releaseNotes->setDontShowVisible(false);
            m_releaseNotes->setUrl(url);
            m_releaseNotes->setWindowTitle(tr("Release Notes"));
            m_somethingToShow = true;
            m_checkNewVersion = false;
        }
        else
        {
            // Si no existeix el fitxer local de les notes, guardem als settings la versió actual
            // per tal de que no es busquin més (no es trobaran) fins a la pròxima versió

            // El write settings guarda la versió de les notes locals només si m_checkNewVersion és fals
            m_checkNewVersion = false;
            // Fem un write settings i no un setCheckFinished per que encara no ha acabat el check
            writeSettings();
            m_checkNewVersion = true;
        }
    }

    if (m_checkNewVersion && !m_dontCheckNewVersionsOnline)
    {
        if (checkTimeInterval())
        {
            delete m_releaseNotes;
            m_releaseNotes = new QReleaseNotes(0);
            m_releaseNotes->setDontShowVisible(true);
            m_releaseNotes->setWindowTitle(tr("New Version Available"));
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
            m_somethingToShow = false;
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

void ApplicationVersionChecker::showReleaseNotes()
{
    // Llegir el contingut dels fitxers HTML de les release notes
    QUrl url = createLocalUrl();
    if (checkLocalUrl(url))
    {
        delete m_releaseNotes;
        m_releaseNotes = new QReleaseNotes(0);
        m_releaseNotes->setDontShowVisible(false);
        m_releaseNotes->setUrl(url);
        m_releaseNotes->setWindowTitle(tr("Release Notes"));
        m_releaseNotes->show();
    }
    else
    {
        ERROR_LOG(QString("No s'ha pogut trobar les release notes de la versió actual al path %1").arg(url.toString()));
        QMessageBox messageBox;
        messageBox.setText(QString(tr("Release notes for the current version not found.")));
        messageBox.setWindowTitle(QString(tr("Error")));
        messageBox.addButton(tr("Ok"), QMessageBox::YesRole);
        messageBox.exec();
    }
}

QUrl ApplicationVersionChecker::createLocalUrl()
{
    // Agafar el path del fitxer
    QString defaultPath = qApp->applicationDirPath() + "/releasenotes/";
    // TODO: si en mode desenvolupament no s'han de veure les release notes, això es pot treure.
    if (!QFile::exists(defaultPath))
    {
        /// Mode desenvolupament
        defaultPath = qApp->applicationDirPath() + "/../releasenotes/";
    }

    QUrl result("");

    QStringList versionList = udg::StarviewerVersionString.split(QString("."));
    if (versionList.count() > 2)
    {
        if (versionList[2].contains("-"))
        {
            versionList[2] = versionList[2].split("-")[0];
        }
        QString version(versionList[0] + "." + versionList[1] + "." + versionList[2]);
        result = defaultPath + "releasenotes" + version + ".html";
    }

    return result;
}

bool ApplicationVersionChecker::checkLocalUrl(const QUrl &url)
{
    // Comprobar si existeix localment el fitxer
    return QFile::exists(QString(url.toString()));
}

bool ApplicationVersionChecker::checkTimeInterval()
{
    // S'utilitzen dues dates, l'actual i la última comprobada
    QDate today = QDate::currentDate();
    QDate lastTime = QDate::fromString(m_lastVersionCheckedDate, QString("yyyyMMdd"));

    int i = m_checkVersionInterval;

    // Si la data que hi ha guardada a les settings és correcte
    if (lastTime.isValid())
    {
        // Calcular quans dies han passat des de que es va comprobar
        i = lastTime.daysTo(today);
    }
    // Si la data no es correcte llavors i = interval i es tornarà a comprobar

    // Retornar si fa tants dies o més com marca l'interval que no s'ha comprobat
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
    while (!found && index < QNetworkInterface::allInterfaces().count())
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
        while (!found && index < QNetworkInterface::allInterfaces().count())
        {
            QNetworkInterface interface = QNetworkInterface::allInterfaces()[index++];

            QNetworkInterface::InterfaceFlags flags = interface.flags();
            bool flagsOk = flags.testFlag(QNetworkInterface::IsUp) && flags.testFlag(QNetworkInterface::IsRunning) &&
                           !flags.testFlag(QNetworkInterface::IsLoopBack);

            // Per si de cas el bluetooth està engegat i foncionant, fer que no l'agafi
            // Rarament trobarem una connexió de xarxa que vagi a través d'un dispositiu bluetooth
            if (interface.isValid() && flagsOk && !interface.humanReadableName().toLower().contains("bluetooth"))
            {
                macAdress += interface.hardwareAddress();
                found = true;
            }
        }
    }

// En cas que estem a windows, Busquem el groupID
#ifdef WIN32
    domain = QProcessEnvironment::systemEnvironment().value(QString("USERDOMAIN"), QString(""));
#endif

    QString machineID = encryptBase64Url(macAdress);
    QString groupID = encryptBase64Url(domain);

    return QString("http://starviewer.udg.edu/checknewversion/?currentVersion=%1&machineID=%2&groupID=%3")
              .arg(StarviewerVersionString).arg(machineID).arg(groupID);
}

QString ApplicationVersionChecker::encryptBase64Url(const QString &url)
{
    return QString(QCryptographicHash::hash(url.toAscii(), QCryptographicHash::Sha1).toBase64().replace("=", "").replace("+", "-").replace("/", "_"));
}

void ApplicationVersionChecker::setProxy(const QUrl &url)
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
        // Ja no es mostren més fins la proxima actualització
        QStringList version = udg::StarviewerVersionString.split(".");
        // Si la versió actual es de debug (això passa quan LastReleaseNotesVersionShown no te valor
        // i es mostren les release notes per primer cop, podria ser que estiguessim en debug)
        if (version.count() > 3)
        {
            settings.setValue(CoreSettings::LastReleaseNotesVersionShown,
                              version[0]+ "." + version[1] + "." + version[2]);
        }
        else
        {
            settings.setValue(CoreSettings::LastReleaseNotesVersionShown, udg::StarviewerVersionString);
        }
    }
    else
    {
        // Guardar la data en que hem comprobat la versió
        settings.setValue(CoreSettings::LastVersionCheckedDate, QDate::currentDate().toString(QString("yyyyMMdd")));
        // Guardar la versió que hem comprobat
        settings.setValue(CoreSettings::LastVersionChecked, m_checkedVersion);
    }
}

void ApplicationVersionChecker::readSettings()
{
    Settings settings;
    m_lastReleaseNotesVersionShown = settings.getValue(CoreSettings::LastReleaseNotesVersionShown).toString();
    m_neverShowNewVersionReleaseNotes = settings.getValue(CoreSettings::NeverShowNewVersionReleaseNotes).toBool();
    m_lastVersionCheckedDate = settings.getValue(CoreSettings::LastVersionCheckedDate).toString();
    m_lastVersionChecked = settings.getValue(CoreSettings::LastVersionChecked).toString();
    m_checkVersionInterval = settings.getValue(CoreSettings::CheckVersionInterval).toInt();
    m_dontCheckNewVersionsOnline = settings.getValue(CoreSettings::DontCheckNewVersionsOnline).toBool();
}

void ApplicationVersionChecker::setCheckFinished()
{
    m_checkFinished = true;
    writeSettings();
    emit checkFinished();
}

bool ApplicationVersionChecker::isNewVersionInstalled()
{
    QStringList lastVersionShown = m_lastReleaseNotesVersionShown.split(".");
    QStringList currentVersion = udg::StarviewerVersionString.split(".");

    QRegExp regularExpression("^[0-9]+\\.[0-9]+\\.[0-9]+(\\-[a-zA-Z]+[0-9]*)?$", Qt::CaseSensitive);

    // Si lastVersionShown no és vàlida (buida o mal formada)
    if (!regularExpression.exactMatch(m_lastReleaseNotesVersionShown))
    {
        return true;
    }
    // Si la versió actual no s'ajusta a l'expressió regular, no farem res.
    else if (!regularExpression.exactMatch(udg::StarviewerVersionString))
    {
        return false;
    }

    // Comparar la primera component
    if (currentVersion[0].toInt() != lastVersionShown[0].toInt())
    {
        return currentVersion[0].toInt() > lastVersionShown[0].toInt();
    }
    // Si son iguals, comparar la segona component
    if (currentVersion[1].toInt() != lastVersionShown[1].toInt())
    {
        return currentVersion[1].toInt() > lastVersionShown[1].toInt();
    }
    // Si també són iguals, comparar la tercera
    // Si no tenen subversio (alpha, beta, RC, etc.)
    if (!lastVersionShown[2].contains("-") && !currentVersion[2].contains("-"))
    {
        return currentVersion[2].toInt() > lastVersionShown[2].toInt();
    }
    else
    {
        return compareVersions(currentVersion[2], lastVersionShown[2]);
    }
}

bool ApplicationVersionChecker::compareVersions(const QString &current, const QString &lastShown)
{
    QStringList currentSplit = current.split("-");
    QStringList lastShownSplit = lastShown.split("-");
    // Comparar la primera component
    if (currentSplit[0].toInt() != lastShownSplit[0].toInt())
    {
        return currentSplit[0].toInt() > lastShownSplit[0].toInt();
    }
    // Si algun té la segona part buida, llavors és major (0.9.1 > 0.9.1-RC1)
    else if (currentSplit.count() != lastShownSplit.count())
    {
        return currentSplit.count() < lastShownSplit.count();
    }
    // Si no, comparar les subversions, alpha, beta, RC, etc.
    else
    {
        QString currentSplitType = getVersionAttribute(currentSplit[1], "type").toLower();
        QString lastShownSplitType = getVersionAttribute(lastShownSplit[1], "type").toLower();
        // Si no son del mateix tipus, es retorna alfabèticament, es tenen en compte alpha, beta i rc
        if (currentSplitType != lastShownSplitType)
        {
            return currentSplitType.compare(lastShownSplitType) > 0;
        }
        // Si són iguals, compararem el numero
        return getVersionAttribute(currentSplit[1], "number").toInt()
                > getVersionAttribute(lastShownSplit[1], "number").toInt();
    }
}

QString ApplicationVersionChecker::getVersionAttribute(const QString &version, const QString &attribute)
{
    if (attribute == "type")
    {
        return QString(version).remove(QRegExp("[0-9]"));
    }
    else if (attribute == "number")
    {
        // Si no hi ha numero no passa res, passar una string buida a int retorna 0.
        return QString(version).remove(QRegExp("[a-zA-Z]"));
    }
    // Si no es vol obtenir ni tipus ni numero retornem la string tal qual
    return version;
}

bool ApplicationVersionChecker::isDevelopmentMode()
{
    QStringList currentVersion = udg::StarviewerVersionString.split(".");
    // Si la versio actual té tres o més parts i conté devel (0.9.1-devel o 0.9.1.devel)
    if (currentVersion.count() > 3)
    {
        return true;
    }
    else if (currentVersion.count() == 3)
    {
        return getVersionAttribute(currentVersion[2], "type").toLower().contains("devel");
    }
    else
    {
        return false;
    }
}

void ApplicationVersionChecker::webServiceReply(QNetworkReply *reply)
{
    // Desconnectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(webServiceReply(QNetworkReply*)));

    bool result = reply->error() == QNetworkReply::NoError;
    // Si no error
    if (result) 
    {
        QString json(reply->readAll());

        QString version("");
        QString releaseNotesURL("");

        QScriptValue scriptValue;
        QScriptEngine engine;
        scriptValue = engine.evaluate("(" + json + ")");

        if (scriptValue.property("error").isObject())
        {
            ERROR_LOG(QString("Error llegint la resposta del servidor (error en el json) ") + scriptValue.property("error").property("code").toString() +
                               QString(": ") + scriptValue.property("error").property("message").toString());
            setCheckFinished();
        }
        else
        {
            if (scriptValue.property("updateAvailable").isBool() && scriptValue.property("updateAvailable").toBool() == true)
            {
                m_checkedVersion = scriptValue.property("version").toString();
                releaseNotesURL = scriptValue.property("releaseNotesURL").toString();

                connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNotesUrlReply(QNetworkReply*)));
                m_manager->get(QNetworkRequest(QUrl(releaseNotesURL)));

                INFO_LOG(QString("S'ha trobat una nova versió en el servidor, %1.").arg(m_checkedVersion));
            }
            else
            {
                INFO_LOG("Starviewer està actualitzat. No s'ha trobat cap versió nova al servidor.");
                setCheckFinished();
            }
        }
    }
    else
    {
        ERROR_LOG(QString("Error buscant noves versions al server. La resposta del webservice és del tipus ") +
                  QString::number(reply->error()) + QString(": ") + reply->errorString());
        setCheckFinished();
    }
    reply->deleteLater();
}

void ApplicationVersionChecker::updateNotesUrlReply(QNetworkReply *reply)
{
    // Desconnectar el manager
    disconnect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNotesUrlReply(QNetworkReply*)));

    bool result = reply->error() == QNetworkReply::NoError;
    // Si no error
    if (result)
    {
        if (m_lastVersionChecked != m_checkedVersion)
        {
            m_releaseNotes->setUrl(reply->url());
            if (!m_neverShowNewVersionReleaseNotes)
            {
                m_somethingToShow = true;
                INFO_LOG(QString("Es mostren les notes: %1").arg(reply->url().toString()));
            }
            else
            {
                INFO_LOG("No es mostren les release notes perquè l'usuari no ho vol");
            }
        }
        else
        {
            INFO_LOG("No es mostren les notes de la nova versió del servidor, per que ja s'han mostrat anteriorment");
        }
    }
    else
    {
        ERROR_LOG(QString("Error en rebre les notes de la versio %1, tipus ").arg(m_checkedVersion) + QString::number(reply->error())+
                  QString(": ") + reply->errorString());
    }
    reply->deleteLater();
    setCheckFinished();
}

void ApplicationVersionChecker::showWhenCheckFinished()
{
    if (m_somethingToShow)
    {
        m_releaseNotes->show();
    }
}

}; // End namespace udg
