/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

/*!
    Aquest arxiu conté totes les macros per a fer logs en l'aplicació.
*/

// include log4cxx header files.
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#define DEBUG

#ifdef DEBUG
// definim el logger de nivell development
log4cxx::LoggerPtr m_developmentLogger( log4cxx::Logger::getLogger("development") );
#endif

// definim el logger de nivell release
log4cxx::LoggerPtr m_releaseLogger( log4cxx::Logger::getLogger("release") );

// log4cxx::BasicConfigurator::configure(); \
/// Macro per a inicialitzar els logger
#define LOGGER_INIT \
{ \
    std::string file = "log4cxx.properties"; \
   log4cxx::PropertyConfigurator::configure( file ); \
}

// if ( fileExist( filename ) )
// {
//         // Si el fitxer de configuració existeix
//         PropertyConfigurator::configure( filename );
// }
// else
// {
//         // configuració per defecte, quin remei!
//         BasicConfigurator::configure();
// }

/// Macro per a missatges de debug
#ifdef DEBUG 
#define DEBUG_LOG( msg ) LOG4CXX_DEBUG( m_developmentLogger , msg )
#else
#define DEBUG_LOG( msg )
#endif

/// Macro per a missatges d'informació general
#define INFO_LOG( msg ) LOG4CXX_INFO( m_releaseLogger , msg )

/// Macro per a missatges de warning
#define WARN_LOG( msg ) LOG4CXX_WARN( m_releaseLogger , msg )

/// Macro per a missatges d'error
#define ERROR_LOG( msg ) LOG4CXX_ERROR( m_releaseLogger , msg )

/// Macro per a missatges d'error fatals/crítics
#define FATAL_LOG( msg ) LOG4CXX_FATAL( m_releaseLogger , msg ); 

