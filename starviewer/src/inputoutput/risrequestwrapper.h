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

#ifndef UDGRISREQUESTWRAPPER_H
#define UDGRISREQUESTWRAPPER_H

#include <QString>

namespace udg {

/**
    Aquesta classe és l'encarregada de fer la integració amb el SAP del ICS per a la descàrrega d'estudis, per fer-ho
    aprofitem la part feta al Starviewer d'integració amb el RIS PIER.
    Aquesta classe envia via TCP/IP una petició al Starviewer Local de descàrrega d'un estudi en el mateix format XML que ho
    fa el RIS PIER, d'aquesta manera aprofitant la part d'integració RIS PIER implementada del Starviewer tenim feta
    la integració amb el SAP.
  */
class RISRequestWrapper {

public:
    /// Envia via Tcp/IP una petició de descarrega de l'estudi amb l'accession number passat per paràmetre al Starviewer de la màquina local
    void sendRequestToLocalStarviewer(QString accessionNumber);

private:
    /// Donant l'accession number retorna un xml que l'Starviewer una vegada l'ha rebut és capaç de descarregar de forma automàtica l'estudi amb l'accession
    /// number passat per paràmetre
    QString getXmlPier(QString accessionNumber);

    /// Imprimeix per pantalla i loggeja error al connectar
    void errorConnecting(int port, QString errorDescription);

    /// Imprimeix per pantalla i loggeja error al enviar la petició per la connexió Tcp/Ip
    void errorWriting(QString errorDescription);

    /// Imprimeix per pantalla i loggeja error al tancar connexió
    void errorClosing(QString errorDescription);
};

}

#endif
