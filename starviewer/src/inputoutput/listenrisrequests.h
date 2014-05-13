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


#ifndef UDGLISTENRISREQUESTS_H
#define UDGLISTENRISREQUESTS_H

#include <QObject>

#include "dicommask.h"

class QTcpServer;

namespace udg {

/**
    Classe que s'encarrega d'escolta per un port especificat a la configuració peticions d'un RIS i atendre les peticions d'aquests
  */
class ListenRISRequests : public QObject {
Q_OBJECT
public:
    enum ListenRISRequestsError { RisPortInUse, UnknownNetworkError };

    ListenRISRequests();

public slots:
    /// Inicia l'escolta de peticions del RIS a través del port que s'ha establet a la configuració
    void listen();

    /// Slot que fa que es parin d'escoltar peticions
    void stopListen();

signals:
    /// Signal que indica que s'ha fet una petició per descarregar un estudi
    void requestRetrieveStudy(DicomMask mask);

    /// Signal que s'emet indicant que s'ha produït un error escoltant peticions al RIS
    void errorListening(ListenRISRequests::ListenRISRequestsError);

private slots:
    void newRISRequest();

private:
    /// Processa la petició rebuda del RIS
    void processRequest(QString risRequestData);

    // Fa un signal del mètode error indicant el tipus d'error que s'ha produït
    void networkError(QTcpServer *tcpRISServer);

private:
    /// Indiquem el temps d'espera (en mil·lisegons) per llegir la petició del RIS, sinó arriba en aquest temps fem time out
    static const int TimeOutToReadData;

    QTcpServer *m_tcpRISServer;
};

}

#endif
