/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef NO_CRASH_REPORTER

#include "crashhandler.h"

#include <QString>
#include <QDir>
#include <QCoreApplication>

#ifdef __APPLE__
    #include "src_breakpad/client/mac/handler/exception_handler.h"
#elif defined WIN32
    #include "src_breakpad/client/windows/handler/exception_handler.h"
#elif defined __linux__
    #include "src_breakpad/client/linux/handler/exception_handler.h"
#endif


#ifndef WIN32

#include <unistd.h>
#include <string>
#include <iostream>

bool launchCrashReporter( const char* dumpDirPath, const char* minidumpId, void* crashHandler, bool succeeded )
{
    // DON'T USE THE HEAP!!!
    // So crashHandler indeed means, no QStrings, no qDebug(), no QAnything, seriously!
    
    if (!succeeded)
        return false;

    pid_t pid = fork();

    if (pid == -1) // fork failed
        return false;
    if (pid == 0) { // we are the fork
        execl( static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath(),
               static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath(),
               dumpDirPath,
               minidumpId,
               (char*) 0 );
        // execl replaces this process, so no more code will be executed
        // unless it failed. If it failed, then we should return false.
        return false;
    }

    // we called fork()
    return true;
}

#else
static bool launchCrashReporter(const wchar_t* dumpDirPath, const wchar_t* minidumpId, void* crashHandler, EXCEPTION_POINTERS *exinfo,
                                MDRawAssertionInfo *assertion, bool succeeded)
{
    if (!succeeded)
        return false;

    // DON'T USE THE HEAP!!!
    // So crashHandler indeed means, no QStrings, no qDebug(), no QAnything, seriously!

    const char* crashReporterPath = static_cast<CrashHandler*>(crashHandler)->getCrashReporterPath();

    //convert crashReporterPath to widechars, which sadly means the product name must be Latin1    
    wchar_t crashReporterPathWchar[ 256 ];
    char* out = (char*)crashReporterPathWchar;
    const char* in = crashReporterPath - 1;
    do {
        *out++ = *++in; //latin1 chars fit in first byte of each wchar
        *out++ = '\0';  //every second byte is NULL
    }
    while (*in);

    wchar_t command[MAX_PATH * 3 + 6];
    wcscpy_s( command, crashReporterPathWchar);
    wcscat_s( command, L" \"" );
    wcscat_s( command, dumpDirPath );
    wcscat_s( command, L"\" \"" );
    wcscat_s( command, minidumpId );
    wcscat_s( command, L"\"" );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    ZeroMemory( &pi, sizeof(pi) );

    if (CreateProcess( NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        TerminateProcess( GetCurrentProcess(), 1 );
    }

    return false;
}

#endif // WIN32

CrashHandler::CrashHandler()
{
#ifndef NO_CRASH_REPORTER
    // primer comprovem que existeixi el directori ~/.starviewer/dumps/ on guradarem els dumps
    QDir dumpsDir = QDir::homePath() + "/.starviewer/dumps/";
    if( !dumpsDir.exists() )
    {
        // creem el directori
        dumpsDir.mkpath( dumpsDir.absolutePath() );
    }

    QString crashReporterPath = QCoreApplication::applicationDirPath() + "/" + STARVIEWER_CRASH_REPORTER_EXE;

    crashReporterPathByteArray = crashReporterPath.toLocal8Bit();
    const char *crashReporterPathCString = crashReporterPathByteArray.constData();

    this->setCrashReporterPath(crashReporterPathCString);
	
#ifndef WIN32
	exceptionHandler = new google_breakpad::ExceptionHandler(dumpsDir.absolutePath().toStdString(), 0, launchCrashReporter, this, true);
#else
    exceptionHandler = new google_breakpad::ExceptionHandler(dumpsDir.absolutePath().toStdWString(), 0, launchCrashReporter, this, google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif //WIN32

#endif //NO_CRASH_REPORTER
}

void CrashHandler::setCrashReporterPath( const char* path )
{
    m_crashReporterPath = path;
}

const char* CrashHandler::getCrashReporterPath() const
{
    return m_crashReporterPath;
}

#endif // NO_CRASH_REPORTER
