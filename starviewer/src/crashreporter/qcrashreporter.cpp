/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <QDateTime>

#include "qcrashreporter.h"
#include "crashreportersender.h"
#include "../core/starviewerapplication.h"
#include "logging.h"
#include <QMovie>
#include <QNetworkInterface>

#ifdef WIN32
    #include <Windows.h>
#endif

namespace udg {

QCrashReporter::QCrashReporter( const QStringList& args , QWidget *parent )
    : QWidget( parent )
{
    setWindowIcon(QIcon(":/images/starviewer.png"));

    setupUi(this);
    
    setWindowTitle( tr("%1").arg( ApplicationNameString ) );
    m_quitPushButton->setText( tr("Quit %1").arg( ApplicationNameString ) );
    m_restartPushButton->setText( tr("Restart %1").arg( ApplicationNameString ) );
    m_sendReportCheckBox->setText( tr("Tell %1 about this crash").arg( ApplicationNameString ) );
    m_informationLabel->setText( tr("We're sorry %1 had a problem and crashed. Please take a moment to send us a crash report to help us diagnose and fix the problem. Your personal information is not sent with this report.  ").arg( ApplicationNameString ) );

    m_minidumpPath = args[1] + "/" + args[2] + ".dmp";
    
    QMovie *sendReportAnimation = new QMovie(this);
    sendReportAnimation->setFileName(":/images/loader.gif");
    m_sendReportAnimation->setMovie(sendReportAnimation);
    sendReportAnimation->start();
    
    m_sendReportLabel->hide();
    m_sendReportAnimation->hide();


    // Busquem les adreces IP del host.
    QString ipAddresses(""); 

    foreach(QNetworkInterface inter, QNetworkInterface::allInterfaces())
    {
        if (inter.flags().testFlag(QNetworkInterface::IsRunning))
        {
            foreach (QNetworkAddressEntry entry, inter.addressEntries())
            {
                if ( inter.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".") && entry.ip().toString() != "127.0.0.1")
                {
                    if ( !ipAddresses.isEmpty() )
                        ipAddresses += ", ";

                    ipAddresses += entry.ip().toString();
                }
            }
        }
    }

    QString hostInformation;
    hostInformation = QString("%1: %2").arg(tr("IP")).arg(ipAddresses);

    // En cas que estem a windows, afegim com a info del host el domini de l'usuari.
#ifdef WIN32
    char * userDomain = getenv ("USERDOMAIN");
    if ( userDomain != NULL )
        hostInformation += QString("\n%1: %2").arg( tr("User Domain") ).arg( userDomain );    
#endif 

    m_hostInformationTextEdit->setPlainText( hostInformation );

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
        m_sendReportAnimation->show();
        m_sendReportLabel->show();
        qApp->processEvents();
        sendReport();
    }
}

void QCrashReporter::sendReport()
{
    QHash<QString,QString> options;
    options.insert( "BuildID", "2009041400" );
    options.insert( "ProductName", ApplicationNameString );
    options.insert( "Version", StarviewerVersionString );
    options.insert( "Email", m_emailLineEdit->text() );

    QString comments( m_descriptionTextEdit->toPlainText() );
    if ( m_hostInformationCheckBox->isChecked() )
    {
        comments += QString("\n// %1:\n").arg(tr("Host information"));
        comments += m_hostInformationTextEdit->toPlainText();
    }

    options.insert( "Comments", comments );
    options.insert( "CrashTime", QByteArray::number( QDateTime::currentDateTime().toTime_t() ) );
    // El valor 1 significa que es tindran en compte les condicions d'acceptacio programades al fitxer
    // de configuracio del "collector".
    // El valor 0 significa que s'acceptaran tots els reports. Les condicions d'acceptacio no es tenen en compte.
    options.insert( "Throttleable", "1" );

    // Enviem el report només en cas de release.
#ifdef QT_NO_DEBUG
    bool success = CrashReporterSender::sendReport("http://starviewer.udg.edu/crashreporter/submit", m_minidumpPath, options);

    if ( success )
    {
        INFO_LOG( QString("Crash report enviat: %1").arg(m_minidumpPath) );
    }
    else
    {
        ERROR_LOG( QString("Error al enviar el crash report: %1").arg(m_minidumpPath) );
    }
#endif

    m_sendReportAnimation->hide();
    m_sendReportLabel->hide();
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
