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

    /// Es fa la connexió connexió
    if (connectToPACS(&pacsConnection))
    {
        /// Es fa un echo al pacs
        OFCondition condition = echoToPACS(&pacsConnection);
        
        /// Desconnexió
        disconnectFromPACS(&pacsConnection);
        
        if (condition.good())
        {
            m_lastError = EchoOk;
        }
        else
        {
            ERROR_LOG("Doing echo to " + pacsDevice.getAETitle() + " does not respond correctly. Error description: " + condition.text());
            m_lastError = EchoFailed;
        }
    }
    else
    {
        INFO_LOG("Doing echo to " + pacsDevice.getAETitle() + " does not respond.");
        m_lastError = EchoCanNotConnectToPACS;
    }

    return m_lastError == EchoOk;
}

EchoToPACS::EchoRequestStatus EchoToPACS::getLastError()
{
    return m_lastError;
}

bool EchoToPACS::connectToPACS(PACSConnection *pacsConnection)
{
    return pacsConnection->connectToPACS(PACSConnection::Echo);
}

OFCondition EchoToPACS::echoToPACS(PACSConnection *pacsConnection)
{
    // Generate next message ID
    DIC_US id = pacsConnection->getConnection()->nextMsgID++;
    // Segons el PS 3.7 apartat 9.1.5.1.4 de DICOM l'status només pot ser 0x0000 si s'ha aconseguit connectar, sinó no hauria de tenir valor
    DIC_US status;
    DcmDataset *dcmDataset = NULL;

    OFCondition condition = DIMSE_echoUser(pacsConnection->getConnection(), id, DIMSE_BLOCKING, 0, &status, &dcmDataset);

    // We don't care about status detail
    delete dcmDataset;
    
    return condition;
}

void EchoToPACS::disconnectFromPACS(PACSConnection *pacsConnection)
{
    pacsConnection->disconnect();
}

}
