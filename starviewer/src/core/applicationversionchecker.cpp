#include "applicationversionchecker.h"
#include "applicationupdatechecker.h"
#include "coresettings.h"
#include "starviewerapplication.h"
#include "logging.h"
#include "machineidentifier.h"

#include <QUrl>
#include <QFile>
#include <QDate>
#include <QStringList>
#include <QMessageBox>

namespace udg {

ApplicationVersionChecker::ApplicationVersionChecker(QObject *parent)
: QObject(parent)
{
    // Per defecte diem que volem comprobar la nova versió, si no, ja es canvia
    m_checkNewVersion = true;
    // Inicialitzem la versio que ens retorna el servidor a cadena buida
    m_checkedVersion = QString("");
    // Inicialitzem m_releaseNotes i el checker online
    m_releaseNotes = new QReleaseNotes();
    m_applicationUpdateChecker = new ApplicationUpdateChecker(this);
}

ApplicationVersionChecker::~ApplicationVersionChecker()
{
    // Destruir la finestra de les release notes
    delete m_releaseNotes;
}

void ApplicationVersionChecker::checkReleaseNotes()
{
    m_checkNewVersion = true;
    m_checkFinished = false;
    m_urlToShow = QUrl("");
    bool checkingOnlineNotes = false;

    // Precondició:
    if (isDevelopmentMode())
    {
        // En mode desenvolupament no es mostraran les notes ni es faran comprovacions online
        m_checkFinished = true;
        return;
    }

    // Utilitzar els settings
    readSettings();
    // En cas de que no hi hagi nova versió al server guardarem l'actual als settings
    m_checkedVersion = m_lastVersionChecked;    

    if (isNewVersionInstalled())
    {
        // Si hi ha una nova versió instalada, no fem un check online
        m_checkNewVersion = false;
        // Llegir el contingut dels fitxers HTML de les release notes
        QUrl url = createLocalUrl();
        if (checkLocalUrl(url))
        {
            m_releaseNotes->setDontShowVisible(false);
            m_releaseNotes->setWindowTitle(tr("Release Notes"));
            m_urlToShow = url;
        }
        else
        {
            // Si no existeix el fitxer local de les notes, guardem als settings la versió actual
            // per tal de que no es busquin més (no es trobaran) fins a la pròxima versió
            writeSettings();
            // Si en local no existeixen, ara sí que les buscarem online
            m_checkNewVersion = true;
        }
    }

    if (m_checkNewVersion && checkTimeInterval())
    {
        // Si no es mostra res en local, i hem de buscar versions on-line
        if (m_dontCheckNewVersionsOnline)
        {
            INFO_LOG("No es busquen les release notes perquè les peticions online estan deshabilitades");
        }
        else
        {
            // Preparem les release notes i fem la crida online a través d'ApplicationVersionCheckerOnServer
            m_releaseNotes->setDontShowVisible(true);
            m_releaseNotes->setWindowTitle(tr("New Version Available"));
            connect(m_applicationUpdateChecker, 
                    SIGNAL(checkFinished()),
                    this, SLOT(onlineCheckFinished()));
            m_applicationUpdateChecker->checkForUpdates();
            checkingOnlineNotes = true;
        }
    }

    if (!checkingOnlineNotes)
    {
        // Si no es busquen actualitzacions online, el check ja ha acabat
        m_checkFinished = true;
        writeSettings();
    }
}

void ApplicationVersionChecker::showIfCorrect()
{
    if (m_checkFinished)
    {
        // Aqui basicament hi entra quan mostra les notes locals
        if (!m_urlToShow.isEmpty())
        {
            m_releaseNotes->showIfUrlLoadsSuccessfully(m_urlToShow);
            m_urlToShow = QUrl("");
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

void ApplicationVersionChecker::showLocalReleaseNotes()
{
    // Llegir el contingut dels fitxers HTML de les release notes
    QUrl url = createLocalUrl();
    if (checkLocalUrl(url))
    {
        m_releaseNotes->setDontShowVisible(false);
        m_releaseNotes->setWindowTitle(tr("Release Notes"));
        m_releaseNotes->showIfUrlLoadsSuccessfully(url);
    }
    else
    {
        ERROR_LOG(QString("No s'ha pogut trobar les release notes de la versió actual al path %1").arg(url.toString()));
        QMessageBox messageBox;
        messageBox.setText(QString(tr("Release notes for the current version not found.")));
        messageBox.setWindowTitle(QString(tr("Error")));
        messageBox.addButton(tr("OK"), QMessageBox::YesRole);
        messageBox.exec();
    }
}

void ApplicationVersionChecker::onlineCheckFinished()
{
    m_checkFinished = true;
    if (m_applicationUpdateChecker->isNewVersionAvailable())
    {
        m_checkedVersion = m_applicationUpdateChecker->getVersion();
        if (m_lastVersionChecked != m_checkedVersion)
        {
            if (m_neverShowNewVersionReleaseNotes)
            {
                INFO_LOG("No es mostren les release notes perquè l'usuari no ho vol");
            }
            else
            {
                m_urlToShow = m_applicationUpdateChecker->getReleaseNotesUrl();
                INFO_LOG(QString("Es mostren les notes: %1").arg(m_applicationUpdateChecker->getReleaseNotesUrl()));
            }
        }
        else
        {
            INFO_LOG("No es mostren les notes de la nova versió del servidor, per que ja s'han mostrat anteriorment");
        }
    }
    else
    {
        if (m_applicationUpdateChecker->isOnlineCheckOk())
        {
            INFO_LOG("Starviewer està actualitzat. No s'ha trobat cap versió nova al servidor.");
        }
        // Si el check no ha anat bé, ja es fa un error log dins de l'application update checker.
    }
    writeSettings();
    emit checkFinished();
}

QUrl ApplicationVersionChecker::createLocalUrl()
{
    // Agafar el path del fitxer
    QString defaultPath = qApp->applicationDirPath() + "/releasenotes/";
    if (!QFile::exists(defaultPath))
    {
        /// Mode desenvolupament (només es veuran notes locals des del menu ajuda -> mostrar notes de la versió)
        defaultPath = qApp->applicationDirPath() + "/../releasenotes/";
    }

    QUrl result("");

    QStringList versionList = StarviewerVersionString.split(QString("."));
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

void ApplicationVersionChecker::writeSettings()
{
    Settings settings;
    if (!m_checkNewVersion)
    {
        // Ja no es mostren més fins la proxima actualització
        QStringList version = StarviewerVersionString.split(".");
        // Si la versió actual es de debug (això passa quan LastReleaseNotesVersionShown no te valor
        // i es mostren les release notes per primer cop, podria ser que estiguessim en debug)
        if (version.count() > 3)
        {
            settings.setValue(CoreSettings::LastReleaseNotesVersionShown,
                              version[0] + "." + version[1] + "." + version[2]);
        }
        else
        {
            settings.setValue(CoreSettings::LastReleaseNotesVersionShown, StarviewerVersionString);
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

bool ApplicationVersionChecker::isNewVersionInstalled()
{
    QStringList lastVersionShown = m_lastReleaseNotesVersionShown.split(".");
    QStringList currentVersion = StarviewerVersionString.split(".");

    QRegExp regularExpression("^[0-9]+\\.[0-9]+\\.[0-9]+(\\-[a-zA-Z]+[0-9]*)?$", Qt::CaseSensitive);

    // Si lastVersionShown no és vàlida (buida o mal formada)
    if (!regularExpression.exactMatch(m_lastReleaseNotesVersionShown))
    {
        return true;
    }
    // Si la versió actual no s'ajusta a l'expressió regular, no farem res.
    else if (!regularExpression.exactMatch(StarviewerVersionString))
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
        QString currentSplitType = getVersionAttribute(currentSplit[1], ApplicationVersionChecker::VersionType).toLower();
        QString lastShownSplitType = getVersionAttribute(lastShownSplit[1], ApplicationVersionChecker::VersionType).toLower();
        // Si no son del mateix tipus, es retorna alfabèticament, es tenen en compte alpha, beta i rc
        if (currentSplitType != lastShownSplitType)
        {
            return currentSplitType.compare(lastShownSplitType) > 0;
        }
        // Si són iguals, compararem el numero
        return getVersionAttribute(currentSplit[1], ApplicationVersionChecker::VersionNumber).toInt()
                > getVersionAttribute(lastShownSplit[1], ApplicationVersionChecker::VersionNumber).toInt();
    }
}

QString ApplicationVersionChecker::getVersionAttribute(const QString &version, ApplicationVersionChecker::VersionAttribute attribute)
{
    switch (attribute)
    {
        case ApplicationVersionChecker::VersionType:
            return QString(version).remove(QRegExp("[0-9]"));
        case ApplicationVersionChecker::VersionNumber:
            // Si no hi ha numero no passa res, passar una string buida a int retorna 0.
            return QString(version).remove(QRegExp("[a-zA-Z]"));
        default:
            // Cas improbable, l'atribut no és correcte
            return version;
    }
}

bool ApplicationVersionChecker::isDevelopmentMode()
{
    QStringList currentVersion = StarviewerVersionString.split(".");
    // Si la versio actual té tres o més parts i conté devel (0.9.1-devel o 0.9.1.devel)
    if (currentVersion.count() > 3)
    {
        return true;
    }
    else if (currentVersion.count() == 3)
    {
        return getVersionAttribute(currentVersion[2], ApplicationVersionChecker::VersionType).toLower().contains("devel");
    }
    else
    {
        return false;
    }
}

void ApplicationVersionChecker::showWhenCheckFinished()
{
    if (!m_urlToShow.isEmpty())
    {
        m_releaseNotes->showIfUrlLoadsSuccessfully(m_urlToShow);
        m_urlToShow = QUrl("");
    }
}

} // End namespace udg
