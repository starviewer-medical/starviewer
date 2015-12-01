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

#include "easylogging++.h"

/*!
    Aquest arxiu conté totes les macros per a fer logs en l'aplicació.
*/

/// Macro per a inicialitzar els loggers
/// Assegurar que només es crida una sola vegada, preferiblement crideu-la
/// just després d'incloure el fitxer logging.h al main.cpp.

#define LOGGER_INIT INITIALIZE_EASYLOGGINGPP

/// Macro per a configurar els logger
/// Definim la variable d'entorn que indica la localització
/// dels fitxers de log i llavors llegim la configuració dels logs
#define LOGGER_CONF(confFile, logFile) \
    do \
    { \
        el::Configurations logConfig(confFile.toStdString()); \
        logConfig.setGlobally(el::ConfigurationType::Filename, logFile.toStdString()); \
        el::Loggers::reconfigureAllLoggers(logConfig); \
        DEBUG_LOG("Logging configuration file: " + configurationFile); \
        DEBUG_LOG("Logging output file: " + logFilePath); \
    } while(false)

/// Macro per a missatges de debug. \TODO de moment fem servir aquesta variable de qmake i funciona bé, però podria ser més adequat troba la forma d'afegir
/// una variable pròpia, com per exemple DEBUG
#ifdef QT_NO_DEBUG
#define DEBUG_LOG(msg) while (false)
#else
#define DEBUG_LOG(msg) \
    do \
    { \
        LOG(DEBUG) << qPrintable(QString(msg)); \
    } while (false)
#endif

/// Macro per a missatges d'informació general
#define INFO_LOG(msg) \
    do \
    { \
        LOG(INFO) << qUtf8Printable(QString(msg)); \
    } while (false)

/// Macro per a missatges de warning
#define WARN_LOG(msg) \
    do \
    { \
        LOG(WARNING) << qUtf8Printable(QString(msg)); \
    } while (false)

/// Macro per a missatges d'error
#define ERROR_LOG(msg) \
    do \
    { \
        LOG(ERROR) << qUtf8Printable(QString(msg)); \
    } while (false)

/// Macro per a missatges d'error fatals/crítics
#define FATAL_LOG(msg) \
    do \
    { \
        LOG(FATAL) << qUtf8Printable(QString(msg)); \
    } while (false)

/// Macro per a missatges verbose
#define VERBOSE_LOG(msg) \
    do \
    { \
        LOG(VERBOSE) << qUtf8Printable(QString(msg)); \
    } while (false)

/// Macro per a missatges d'assegurament de la qualitat
#define QA_LOG(msg) \
    do \
    { \
        LOG(QA) << qUtf8Printable(QString(msg)); \
    } while (false)

/// Macro per a missatges de traca
#define TRACE_LOG(msg) \
    do \
    { \
        LOG(TRACE) << qUtf8Printable(QString(msg)); \
    } while (false)

#endif
