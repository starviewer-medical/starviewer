/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QTcpSocket>
#include <QSettings>
#include <QProcess>
#include <QCoreApplication>
#include <QProcess>
#include <QThread>

#ifdef _WIN32
#include "windows.h"
#endif

#include "../inputoutput/starviewersettings.h"
#include "../core/logging.h"

#include "qstarviewersapwrapper.h"


namespace udg {

void QStarviewerSAPWrapper::sendRequestToLocalStarviewer(QString accessionNumber)
{
    StarviewerSettings settings;
    QTcpSocket tcpSocket;
    QString locaHostAddress = "127.0.0.1";//IP del localhost
    int starviewerRisPort = settings.getListenPortRisRequests();//Port pel que Starviewer espera peticions del RIS

    INFO_LOG(QString("QStarviewerSAPWrapper::Demanare a l'Starviewer local pel port %1 la descarrega de l'estudi amb accession number %2").arg(QString().setNum(starviewerRisPort), accessionNumber));

    tcpSocket.connectToHost(locaHostAddress, starviewerRisPort);//Connectem contra el localhost
    if (!tcpSocket.waitForConnected()) //Esperem que ens haguem connectat
    {
        errorConnecting(starviewerRisPort, tcpSocket.errorString());
        return;
    }

    tcpSocket.write(getXmlPier(accessionNumber).toLocal8Bit()); //Enviem la petició de descarregar del estudi
    if (!tcpSocket.waitForBytesWritten())
    {
        INFO_LOG("QStarviewerSAPWrapper::No s'ha pogut enviar la petició a Starviewer");

        if (tcpSocket.error() != QAbstractSocket::UnknownSocketError)
        {
            errorWriting(tcpSocket.errorString());
        }
    }
    else 
    {
        INFO_LOG("QStarviewerSAPWrapper::S'ha enviat la petició correctament al Starviewer");
        printf(qPrintable(QString("The request to retrieve the study with accession number %1 has been sent succesfully.\n").arg(accessionNumber)));
    }

    tcpSocket.flush();

    tcpSocket.disconnectFromHost();//desconnectem
    if (!tcpSocket.waitForDisconnected())
    {
        INFO_LOG("QStarviewerSAPWrapper::No he pogut desconnectar del Starviewer");
    }
    else INFO_LOG("QStarviewerSAPWrapper::He desconnectat del Starviewer");

    if (tcpSocket.error() != QAbstractSocket::UnknownSocketError)
    {
        errorClosing(tcpSocket.errorString());
    }
}

bool QStarviewerSAPWrapper::isStarviewerRunning()
{
    StarviewerSettings settings;
    QTcpSocket tcpSocket;
    QString locaHostAddress = "127.0.0.1";//IP del localhost
    int starviewerRisPort = settings.getListenPortRisRequests();//Port pel que Starviewer espera peticions del RIS

    INFO_LOG(QString("Comprovo si l'Starviewer està engegat. Intentaré connectar amb l'Starviewer pel port: %1").arg(QString().setNum(starviewerRisPort)));

    tcpSocket.connectToHost(locaHostAddress, starviewerRisPort);//Connectem contra el localhost

    if (!tcpSocket.waitForConnected(1000)) //Esperem que ens haguem connectat
    {
        if (tcpSocket.error() != QAbstractSocket::SocketTimeoutError)
        {
            errorConnecting(starviewerRisPort, tcpSocket.errorString());
        }

        INFO_LOG("QStarviewerSAPWrapper::Starviewer no ha respós, no està engegat");
        return false;
    }

    tcpSocket.close();

    INFO_LOG("QStarviewerSAPWrapper::Starviewer ha, respós està engegat");

    return true;
}

bool QStarviewerSAPWrapper::startStarviewer()
{
    QString starviewerFilePath = getStarviewerExecutableFilePath();
    bool starviewerRunning = false;
    int tries = 0, maxTries = 10;
    QProcess process;

    INFO_LOG("QStarviewerSAPWrapper:Intento engegar Starviewer: " + starviewerFilePath);
    process.startDetached(starviewerFilePath);

    //Fem 10 intents per comprovar si l'Starviewer s'està executant
    while (tries < maxTries && !starviewerRunning)
    {
        sleepCurrentProcess(2);//Adormim dos segons i comprovem si l'Starviewer Respón
        starviewerRunning = isStarviewerRunning();
        tries++;

        if (!starviewerRunning)
            INFO_LOG(QString("QStarviewerSAPWrapper:Intent %1 de %2 Starviewer encara no respón").arg(tries, maxTries));
    }

    return true;
}

QString QStarviewerSAPWrapper::getXmlPier(QString accessionNumber)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?><Msg Name='OpenStudies'><Param Name='AccessionNumber'>" + accessionNumber + "</Param></Msg>";
    return xml;
}

void QStarviewerSAPWrapper::errorConnecting(int starviewerRisPort, QString errorDescription)
{
    QString messageError = QString("Can't connect with Starviewer on port %1, be sure that Starviewer is running. Error description: %2.\n").arg(QString().setNum(starviewerRisPort), errorDescription);

    ERROR_LOG(QString("QStarviewerSAPWrapper::No s'ha pogut connectar amb l'Starviewer pel port %1, descripcio error: %2").arg(QString().setNum(starviewerRisPort), errorDescription));
    printf(qPrintable(messageError));
}

void QStarviewerSAPWrapper::errorWriting(QString errorDescription)
{
    QString messageError = QString("Error can't send the request to Starviewer. Error description:  %1.\n").arg(errorDescription);

    ERROR_LOG("QStarviewerSAPWrapper::No s'ha pogut enviar la peticio al Starviewer, descripcio error: " + errorDescription);
    printf(qPrintable(messageError));
}

void QStarviewerSAPWrapper::errorClosing(QString errorDescription)
{
    QString messageError = QString("Error while disconnecting from host. Error description:  %1.\n").arg(errorDescription);

    ERROR_LOG("QStarviewerSAPWrapper::S'ha produit un error desconnectant del host, descripcio del error: " + errorDescription);
    printf(qPrintable(messageError));
}

void QStarviewerSAPWrapper::sleepCurrentProcess(uint secondsToSleep)
{
    #ifdef _WIN32
    Sleep(secondsToSleep *1000);
    #else
    sleep(secondsToSleep);
    #endif
}

QString QStarviewerSAPWrapper::getStarviewerExecutableFilePath()
{
    #ifdef _WIN32
        /*En windows per poder executar l'starviewer hem de tenir en compte que si està en algun directori que conte espais
         *com el directori C:\Program Files\Starviewer\starviewer.exe, hem de posar el path entre cometes 
         * per a que no ho interpreti com a paràmetres, per exemple "C:\Program Files\Starviewer\starviewer.exe" */

        return "\"" + QCoreApplication::applicationDirPath() + "/starviewer.exe" + "\""; //afegim les cometes per si algun dels directori conté espai
    #else 
        return QCoreApplication::applicationDirPath() + "/starviewer";
    #endif
}

}
