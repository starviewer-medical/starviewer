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


/// Macro per a inicialitzar els loggers
/// Assegurar que només es crida una sola vegada, preferiblement crideu-la
/// just després d'incloure el fitxer logging.h al main.cpp.

namespace udg {
    void beginLogging();
    /**
     * Returns the path where the log should be outputted to.
     * @return Log file path
     */
    QString getLogFilePath();
    /**
     * Location of log.conf file.
     * 
     * This function tries to locate the logging configuration file, trying
     * different fallback alternatives.
     * 
     * @return Path where log.conf is expected to be found
     */
    QString getLogConfFilePath();
    

    void debugLog(const QString &msg, const QString &file, int line, const QString &function);
    void infoLog(const QString &msg, const QString &file, int line, const QString &function);
    void warnLog(const QString &msg, const QString &file, int line, const QString &function);
    void errorLog(const QString &msg, const QString &file, int line, const QString &function);
    void fatalLog(const QString &msg, const QString &file, int line, const QString &function);
    void verboseLog(int vLevel, const QString &msg, const QString &file, int line, const QString &function);
    void traceLog(const QString &msg, const QString &file, int line, const QString &function);
}


//Taken from easylogging++.h
#if _MSC_VER  // Visual C++
    #define LOG_FUNC __FUNCSIG__
#elif __GNUC__  // GCC
    #define LOG_FUNC __PRETTY_FUNCTION__
#elif defined(__clang__) && (__clang__ == 1)  // Clang++
    #define LOG_FUNC __PRETTY_FUNCTION__
#elif __INTEL_COMPILER  // Intel C++
    #define LOG_FUNC __PRETTY_FUNCTION__
#else
    #if defined(__func__)
        #define LOG_FUNC __func__
    #else
        #define LOG_FUNC ""
    #endif
#endif


/// Macro per a missatges de debug. \TODO de moment fem servir aquesta variable de qmake i funciona bé, però podria ser més adequat troba la forma d'afegir
/// una variable pròpia, com per exemple DEBUG
#ifdef QT_NO_DEBUG
    #define DEBUG_LOG(msg) while (false)
#else
    #define DEBUG_LOG(msg) udg::debugLog(msg,__FILE__,__LINE__,LOG_FUNC)
#endif

#define INFO_LOG(msg) udg::infoLog(msg,__FILE__,__LINE__,LOG_FUNC)
#define WARN_LOG(msg) udg::warnLog(msg,__FILE__,__LINE__,LOG_FUNC)
#define ERROR_LOG(msg) udg::errorLog(msg,__FILE__,__LINE__,LOG_FUNC)
#define FATAL_LOG(msg) udg::fatalLog(msg,__FILE__,__LINE__,LOG_FUNC)
#define VERBOSE_LOG(vLevel, msg) udg::verboseLog(vLevel, msg,__FILE__,__LINE__,LOG_FUNC)
#define TRACE_LOG(msg) udg::traceLog(msg,__FILE__,__LINE__,LOG_FUNC)




#endif //_LOGGING_
