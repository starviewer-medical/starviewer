#include "logging.h"
#include "easylogging++.h"
#include "starviewerapplication.h"
#include <QDir>
#include <QApplication>

static bool loggingStarted = false;

void udg::beginLogging() 
{
    // Primer comprovem que existeixi el direcotori ~/.starviewer/log/ on guradarem els logs
    QDir logDir = udg::UserLogsPath;
    if (!logDir.exists())
    {
        // Creem el directori
        logDir.mkpath(udg::UserLogsPath);
    }
    
    el::Configurations logConfig(getLogConfFilePath().toStdString());
    logConfig.setGlobally(el::ConfigurationType::Filename, getLogFilePath().toStdString());
    
    //Disable logging to the standard output when compiled on release
    #ifdef QT_NO_DEBUG
    logConfig.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    #endif
    
    el::Loggers::reconfigureAllLoggers(logConfig);
    loggingStarted = true;
}

void udg::endLogging(int returnValue) 
{
    //Not used
}

QString udg::getLogFilePath() 
{
    return QDir::toNativeSeparators(udg::UserLogsFile);
}

QString udg::getLogConfFilePath() 
{
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
    return configurationFile;
}

void udg::debugLog(const QString &msg, const QString &file, int line, const QString &function) 
{
    if (loggingStarted) 
    {
        ;
        LOG(DEBUG) << qPrintable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
    }
}

void udg::infoLog(const QString &msg, const QString&, int, const QString&) 
{
    if (loggingStarted) 
    {
        LOG(INFO) << qUtf8Printable(msg);
    }
}

void udg::warnLog(const QString &msg, const QString &file, int line, const QString &function) 
{
    if (loggingStarted) 
    {
        LOG(WARNING) << qUtf8Printable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
    }
}

void udg::errorLog(const QString &msg, const QString &file, int line, const QString &function) 
{
    if (loggingStarted) 
    {
        LOG(ERROR) << qUtf8Printable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
    }
}

void udg::fatalLog(const QString &msg, const QString &file, int line, const QString &function) 
{
    if (loggingStarted) 
    {
        LOG(FATAL) << qUtf8Printable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
    }
}

void udg::verboseLog(int vLevel, const QString &msg, const QString&, int, const QString&) 
{
    if (loggingStarted) 
    {
        VLOG(vLevel) << qUtf8Printable(msg);
    }
}

void udg::traceLog(const QString &msg, const QString&, int, const QString&) 
{
    if (loggingStarted) 
    {
        LOG(TRACE) << qUtf8Printable(msg);
    }
}
