/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCRASHHANDLER_H
#define UDGCRASHHANDLER_H

#ifndef NO_CRASH_REPORTER

#include <QString>

namespace google_breakpad
{
class ExceptionHandler;
}

class CrashHandler
{
public:
    CrashHandler();
    
    ~CrashHandler(){};

    void setCrashReporterPath( const char* path );
    const char* getCrashReporterPath() const;

private:
	QByteArray crashReporterPathByteArray;
    const char* m_crashReporterPath;
	google_breakpad::ExceptionHandler *exceptionHandler;
};

#endif // NO_CRASH_REPORTER

#endif // UDGCRASHHANDLER_H
