/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGLISTENRISREQUESTS_H
#define UDGLISTENRISREQUESTS_H

#include <QObject>

#include "dicommask.h"

class QTcpServer;
class QTcpSocket;

namespace udg {

/** Classe que s'encarrega d'escolta per un port especificat a la configuració peticions d'un RIS i atendre les peticions d'aquests
 *
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class ListenRISRequests: public QObject
{
Q_OBJECT
public:

    enum ListenRISRequestsError { RisPortInUse, UnknownNetworkError };

    ListenRISRequests(QObject *parent = 0);

    ///Indica si s'estant escoltant peticions
    bool isListening();

public slots:

    ///Inicia l'escolta de peticions del RIS a través del port que s'ha establet a la configuració
    void listen();

signals:
    ///Signal que indica que s'ha fet una petició per descarregar un estudi
    void requestRetrieveStudy(DicomMask mask);
    
    ///Signal que s'emet indicant que s'ha produït un error escoltant peticions al RIS
    void errorListening(ListenRISRequests::ListenRISRequestsError );

private :
    /// Indiquem el temps d'espera (en mil·lisegons) per llegir la petició del RIS, sinó arriba en aquest temps fem time out
    static const int TimeOutToReadData;

    ///Processa la petició rebuda del RIS
    void processRequest(QString risRequestData);

    //Fa un signal del mètode error indicant el tipus d'error que s'ha produït
    void networkError(QTcpServer *tcpRISServer);

private:

    bool m_isListeningRISRequests;
};

}

#endif
