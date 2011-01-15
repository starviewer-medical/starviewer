#include "echotopacs.h"

#include <dimse.h>
#include <ofcond.h>
#include <assoc.h>

#include "pacsdevice.h"
#include "pacsconnection.h"
#include "logging.h"

namespace udg
{
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
        DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
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