#include "qstarviewersapwrapper.h"
#include "logging.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"

#include <QTcpSocket>
#include <QProcess>
#include <QCoreApplication>

namespace udg {

void QStarviewerSAPWrapper::sendRequestToLocalStarviewer(QString accessionNumber)
{
    Settings settings;
    QTcpSocket tcpSocket;
    // IP del localhost
    QString locaHostAddress = "127.0.0.1";
    // Port pel que Starviewer espera peticions del RIS
    int starviewerRisPort = settings.getValue(InputOutputSettings::RISRequestsPort).toInt();

    INFO_LOG(QString("QStarviewerSAPWrapper::Demanare a l'Starviewer local pel port %1 la descarrega de l'estudi amb accession number %2")
                .arg(QString().setNum(starviewerRisPort), accessionNumber));

    // Connectem contra el localhost
    tcpSocket.connectToHost(locaHostAddress, starviewerRisPort);
    // Esperem que ens haguem connectat
    if (!tcpSocket.waitForConnected())
    {
        errorConnecting(starviewerRisPort, tcpSocket.errorString());
        return;
    }

    // Enviem la petició de descarregar del estudi
    tcpSocket.write(getXmlPier(accessionNumber).toLocal8Bit());
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
    }

    tcpSocket.flush();

    // Desconnectem
    tcpSocket.disconnectFromHost();
    if (tcpSocket.state() != QAbstractSocket::UnconnectedState && !tcpSocket.waitForDisconnected())
    {
        INFO_LOG("QStarviewerSAPWrapper::No he pogut desconnectar del Starviewer");
    }
    else
    {
        INFO_LOG("QStarviewerSAPWrapper::He desconnectat del Starviewer");
    }

    if (tcpSocket.error() != QAbstractSocket::UnknownSocketError)
    {
        errorClosing(tcpSocket.errorString());
    }
}

QString QStarviewerSAPWrapper::getXmlPier(QString accessionNumber)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?><Msg Name='OpenStudies'><Param Name='AccessionNumber'>" + accessionNumber + "</Param></Msg>";
    return xml;
}

void QStarviewerSAPWrapper::errorConnecting(int starviewerRisPort, QString errorDescription)
{
    QString messageError = QString("Unable to connect with %3 on port %1, be sure %3 is running. Error description: %2.\n")
                .arg(QString().setNum(starviewerRisPort), errorDescription, ApplicationNameString);

    ERROR_LOG(QString("QStarviewerSAPWrapper::No s'ha pogut connectar amb l'Starviewer pel port %1, descripcio error: %2")
                 .arg(QString().setNum(starviewerRisPort), errorDescription));
}

void QStarviewerSAPWrapper::errorWriting(QString errorDescription)
{
    QString messageError = QString("Error cannot send the request to %2. Error description:  %1.\n").arg(errorDescription).arg(ApplicationNameString);

    ERROR_LOG("QStarviewerSAPWrapper::No s'ha pogut enviar la peticio al Starviewer, descripcio error: " + errorDescription);
}

void QStarviewerSAPWrapper::errorClosing(QString errorDescription)
{
    QString messageError = QString("Error while disconnecting from host. Error description:  %1.\n").arg(errorDescription);

    ERROR_LOG("QStarviewerSAPWrapper::S'ha produit un error desconnectant del host, descripcio del error: " + errorDescription);
}
}
