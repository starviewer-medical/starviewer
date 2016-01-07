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

#ifndef _LOGGING_
#define _LOGGING_

#include <QString>
#include <QtGlobal> // Pel qpuntenv()
/*!
    Aquest arxiu conté totes les macros per a fer logs en l'aplicació.
*/

// Include log4cxx header files.
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

/// Macro per a inicialitzar els logger
/// Definim la variable d'entorn que indica la localització
/// dels fitxers de log i llavors llegim la configuració dels logs
#define LOGGER_INIT(file) \
    if (true) \
    { \
        QByteArray logFilePathValue = (QDir::toNativeSeparators(udg::UserLogsFile)).toLatin1(); \
        qputenv("logFilePath", logFilePathValue); \
        log4cxx::PropertyConfigurator::configure(file); \
    } else (void)0

/// Macro per a missatges de debug. \TODO de moment fem servir aquesta variable de qmake i funciona bé, però podria ser més adequat troba la forma d'afegir
/// una variable pròpia, com per exemple DEBUG
#ifdef QT_NO_DEBUG
#define DEBUG_LOG(msg) (void)0
#else
#define DEBUG_LOG(msg) \
    if (true) \
    { \
        LOG4CXX_DEBUG(log4cxx::Logger::getLogger("development"), qPrintable(QString(msg))) \
    } else (void)0

#endif

/// Macro per a missatges d'informació general
#define INFO_LOG(msg) \
    if (true) \
    { \
        LOG4CXX_INFO(log4cxx::Logger::getLogger("info.release"), QString(msg).toUtf8().constData()) \
    } else (void)0

/// Macro per a missatges de warning
#define WARN_LOG(msg) \
    if (true) \
    { \
        LOG4CXX_WARN(log4cxx::Logger::getLogger("info.release"), QString(msg).toUtf8().constData()) \
    } else (void)0

/// Macro per a missatges d'error
#define ERROR_LOG(msg) \
    if (true) \
    { \
        LOG4CXX_ERROR(log4cxx::Logger::getLogger("errors.release"), QString(msg).toUtf8().constData()) \
    } else (void)0

/// Macro per a missatges d'error fatals/crítics
#define FATAL_LOG(msg) \
    if (true) \
    { \
        LOG4CXX_FATAL(log4cxx::Logger::getLogger("errors.release"), QString(msg).toUtf8().constData()) \
    } else (void)0

#endif
