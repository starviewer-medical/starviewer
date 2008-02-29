/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef _LOGGING_
#define _LOGGING_

#include <QString>
/*!
    Aquest arxiu conté totes les macros per a fer logs en l'aplicació.
*/

// include log4cxx header files.
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

/// Macro per a inicialitzar els logger
#define LOGGER_INIT( file ) \
{ \
    log4cxx::PropertyConfigurator::configure( file ); \
    INFO_LOG("__________________________________") \
    INFO_LOG("::::::::::::::::::::::::::::::::::") \
    INFO_LOG("* Inicialització de l'aplicació *") \
    INFO_LOG("::::::::::::::::::::::::::::::::::") \
    INFO_LOG("") \
}

/// Macro per a missatges de debug. \TODO de moment fem servir aquesta variable de qmake i funciona bé, però podria ser més adequat troba la forma d'afegir una variable pròpia, com per exemple DEBUG
#ifdef QT_NO_DEBUG
#define DEBUG_LOG( msg );
#else
#define DEBUG_LOG( msg ){ LOG4CXX_DEBUG( log4cxx::Logger::getLogger("development") , qPrintable( QString(msg) ) ) }

#endif

/// Macro per a missatges d'informació general
#define INFO_LOG( msg ){ LOG4CXX_INFO( log4cxx::Logger::getLogger("info.release") , qPrintable( QString(msg) ) ) }

/// Macro per a missatges de warning
#define WARN_LOG( msg ){ LOG4CXX_WARN( log4cxx::Logger::getLogger("info.release") , qPrintable( QString(msg) ) ) }

/// Macro per a missatges d'error
#define ERROR_LOG( msg ){ LOG4CXX_ERROR( log4cxx::Logger::getLogger("errors.release") , qPrintable( QString(msg) ) ) }

/// Macro per a missatges d'error fatals/crítics
#define FATAL_LOG( msg ){ LOG4CXX_FATAL( log4cxx::Logger::getLogger("errors.release") , qPrintable( QString(msg) ) ) }

/// Macro per a missatges d'estadístiques
#define STAT_LOG( msg ){ LOG4CXX_INFO( log4cxx::Logger::getLogger("info.release") , qPrintable( QString("STAT: ") + QString(msg) ) ) }

#endif
