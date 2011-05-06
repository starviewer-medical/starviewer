#include "echotopacs.h"

#include <dimse.h>
#include <ofcond.h>
#include <assoc.h>

#include "pacsdevice.h"
#include "pacsconnection.h"
#include "logging.h"

namespace udg {
EchoToPACS::EchoToPACS()
{
    m_lastError = EchoFailed;
}

bool EchoToPACS::echo(PacsDevice pacsDevice)
{
    PACSConnection pacsConnection(pacsDevice);

    if (pacsConnection.connectToPACS(PACSConnection::Echo))
    {
        DIC_US id = pacsConnection.getConnection()->nextMsgID++; // generate next message ID
        //Segons el PS 3.7 apartat 9.1.5.1.4 de DICOM l'status només pot ser 0x0000 si s'ha aconseguit connectar, sinó no hauria de tenir valor
        DIC_US status;
        DcmDataset *dcmDataset = NULL;

        OFCondition condition = DIMSE_echoUser(pacsConnection.getConnection(), id, DIMSE_BLOCKING, 0, &status, &dcmDataset);

        pacsConnection.disconnect();
        delete dcmDataset; // we don't care about status detail

        if (condition.good())
        {
            m_lastError = EchoOk;
        }
        else
        {
            ERROR_LOG("Doing echo to " + pacsDevice.getAETitle() + " doesn't responds correctly. Error description: " + condition.text());
            m_lastError = EchoFailed;
        }
    }
    else
    {
        INFO_LOG("Doing echo to " + pacsDevice.getAETitle() + " doesn't responds.");
        m_lastError = EchoCanNotConnectToPACS;
    }

    return m_lastError == EchoOk;
}

EchoToPACS::EchoRequestStatus EchoToPACS::getLastError()
{
    return m_lastError;
}

}
