#include "logging.h"
#include "starviewerapplication.h"
#include <QDir>
#include <QApplication>

void udg::beginLogging() {
    // Primer comprovem que existeixi el direcotori ~/.starviewer/log/ on guradarem els logs
    QDir logDir = udg::UserLogsPath;
    if (!logDir.exists())
    {
        // Creem el directori
        logDir.mkpath(udg::UserLogsPath);
    }
    // TODO donem per fet que l'arxiu es diu així i es troba a la localització que indiquem. S'hauria de fer una mica més flexible o genèric;
    // està així perquè de moment volem anar per feina i no entretenir-nos però s'ha de fer bé.
    QString configurationFile = "/etc/starviewer/log.conf";
    if (!QFile::exists(configurationFile))
    {
        configurationFile = qApp->applicationDirPath() + "/log.conf";
    }
    // Afegim localització per Mac OS X en desenvolupament
    if (!QFile::exists(configurationFile))
    {
        configurationFile = qApp->applicationDirPath() + "/../../../log.conf";
    }

    QString logFilePath = QDir::toNativeSeparators(udg::UserLogsFile);

    el::Configurations logConfig(configurationFile.toStdString());
    logConfig.setGlobally(el::ConfigurationType::Filename, logFilePath.toStdString());

    //Disable logging to the standard output when compiled on release
    #ifdef QT_NO_DEBUG
    logConfig.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    #endif

    el::Loggers::reconfigureAllLoggers(logConfig);

    // Marquem l'inici de l'aplicació al log
    INFO_LOG("==================================================== BEGIN ====================================================");
    INFO_LOG(QString("%1 Version %2 BuildID %3").arg(udg::ApplicationNameString).arg(udg::StarviewerVersionString).arg(udg::StarviewerBuildID));
}

void udg::endLogging(int returnValue) {
    INFO_LOG(QString("%1 Version %2 BuildID %3, returnValue %4").arg(udg::ApplicationNameString).arg(udg::StarviewerVersionString)
             .arg(udg::StarviewerBuildID).arg(returnValue));
    INFO_LOG("===================================================== END =====================================================");
}
