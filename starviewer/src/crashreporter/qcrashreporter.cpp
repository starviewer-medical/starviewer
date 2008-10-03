/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qcrashreporter.h"

#ifdef WIN32
    #include <Windows.h>
#endif

namespace udg {
    
QCrashReporter::QCrashReporter( const QStringList& args , QWidget *parent )
    : QWidget( parent )
{
    setupUi(this);
    
    connect(m_quitPushButton, SIGNAL( clicked() ), this, SLOT( quitButtonClickedSlot() ) );
    connect(m_restartPushButton, SIGNAL( clicked() ), this, SLOT( restartButtonClickedSlot() ) );
}

void QCrashReporter::quitButtonClickedSlot()
{
    maybeSendReport();
    close();
}

void QCrashReporter::restartButtonClickedSlot()
{
    maybeSendReport();

    QString starviewerPath = QCoreApplication::applicationDirPath() + "/" + STARVIEWER_EXE;
    QByteArray starviewerPathByteArray = starviewerPath.toLocal8Bit();
    const char * starviewerPathCString = starviewerPathByteArray.constData();

    restart( starviewerPathCString );

    close();
}

void QCrashReporter::maybeSendReport()
{
    if ( m_sendReportCheckBox->isChecked() )
    {
        sendReport();
    }
}

void QCrashReporter::sendReport()
{

}
    
bool QCrashReporter::restart(const char * path)
{

#ifndef WIN32 

    pid_t pid = fork();
    
    if (pid == -1) // el fork ha fallat
        return false;
    if (pid == 0) { // estem al fork
        execl( path, path, (char*) 0 );
        // execl substitueix aquest proces, per tant el que ve a continuació no s'hauria d'executar
        exit(1);
    }
    
    return true;

#else
    
    //convert path to widechars, which sadly means the path name must be Latin1    
    wchar_t pathWchar[ 256 ];
    char* out = (char*)pathWchar;
    const char* in = path - 1;
    do {
        *out++ = *++in; //latin1 chars fit in first byte of each wchar
        *out++ = '\0';  //every second byte is NULL
    }
    while (*in);
    
    wchar_t command[MAX_PATH * 3 + 6];
    wcscpy_s( command, pathWchar);
    wcscat_s( command, L" \"" );

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
     
    return true;
    
#endif // WIN32
}

};
