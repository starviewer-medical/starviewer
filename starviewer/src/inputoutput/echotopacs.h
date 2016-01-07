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

#ifndef UDGECHOTOPACS_H
#define UDGECHOTOPACS_H

class OFCondition;

namespace udg {

class PacsDevice;
class PACSConnection;

/**
    Classe que s'encarrega de fer un echoSCU a un PACS
  */
class EchoToPACS {
public:
    enum EchoRequestStatus { EchoOk, EchoCanNotConnectToPACS, EchoFailed };

    EchoToPACS();

    /// Fa un echo al PACS passat per paràmetre
    bool echo(PacsDevice pacsDevice);

    /// Retorna l'estat de l'últim echo fet a un PACS
    EchoToPACS::EchoRequestStatus getLastError();

protected:
    virtual bool connectToPACS(PACSConnection *pacsConnection);
    virtual OFCondition echoToPACS(PACSConnection *pacsConnection);
    virtual void disconnectFromPACS(PACSConnection *pacsConnection);

private:
    EchoToPACS::EchoRequestStatus m_lastError;

};

}

#endif
