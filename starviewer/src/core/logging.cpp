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

#include "logging.h"
#include "easylogging++.h"
#include "starviewerapplication.h"

#include <QApplication>

namespace udg {

void beginLogging()
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
}

QString getLogFilePath()
{
    return QDir::toNativeSeparators(udg::UserLogsFile);
}

QString getLogConfFilePath()
{
    // TODO donem per fet que l'arxiu es diu així i es troba a la localització que indiquem. S'hauria de fer una mica més flexible o genèric;
    // està així perquè de moment volem anar per feina i no entretenir-nos però s'ha de fer bé.

    QString configurationFile;

    if (qApp->applicationFilePath().contains("autotests"))
    {
        configurationFile = sourcePath() + "/tests/auto/log.conf";
    }
    else
    {
        configurationFile = "/etc/starviewer/log.conf";

        if (!QFile::exists(configurationFile))
        {
            configurationFile = installationPath() + "/log.conf";
        }
        if (!QFile::exists(configurationFile))
        {
            configurationFile = sourcePath() + "/bin/log.conf";
        }
    }

    return configurationFile;
}

void debugLog(const QString &msg, const QString &file, int line, const QString &function)
{
    LOG(DEBUG) << qPrintable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
}

void infoLog(const QString &msg, const QString&, int, const QString&)
{
    LOG(INFO) << qUtf8Printable(msg);
}

void warnLog(const QString &msg, const QString &file, int line, const QString &function)
{
    LOG(WARNING) << qUtf8Printable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
}

void errorLog(const QString &msg, const QString &file, int line, const QString &function)
{
    LOG(ERROR) << qUtf8Printable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
}

void fatalLog(const QString &msg, const QString &file, int line, const QString &function)
{
    LOG(FATAL) << qUtf8Printable(QString("%1 [ %2:%3 %4 ]").arg(msg).arg(file).arg(line).arg(function));
}

void verboseLog(int vLevel, const QString &msg, const QString&, int, const QString&)
{
    VLOG(vLevel) << qUtf8Printable(msg);
}

void traceLog(const QString &msg, const QString&, int, const QString&)
{
    LOG(TRACE) << qUtf8Printable(msg);
}

}
