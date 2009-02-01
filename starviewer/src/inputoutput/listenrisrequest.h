/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGLISTENRISREQUEST_H
#define UDGLISTENRISREQUEST_H

#include <QObject>

#include "dicommask.h"

class QTcpServer;
class QTcpSocket;

namespace udg {


/** Classe que s'encarrega d'escolta per un port especificat a la configuració peticions d'un RIS 
 *
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class ListenRisRequest : public QObject
{
Q_OBJECT
public:

    ListenRisRequest(QObject *parent = 0);

    void listen();

    bool isListen();

signals:

    ///Signal que indica que s'ha fet una petició per descarregar un estudi
    void requestRetrieveStudy(DicomMask mask);

private slots:

    ///slot que s'activa quan rebem una nova connexió
    void newConnection();

private :

    QTcpServer *m_qTcpServer;

    //Mostra l'error que s'ha produït en el socket
    void showNetworkError();
};

}

#endif
