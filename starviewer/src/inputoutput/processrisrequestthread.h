/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGPROCESSRISREQUESTTHREAD_H
#define UDGPROCESSRISREQUESTTHREAD_H


#include <QObject>
#include <QThread>
#include <QTcpSocket>

#include "dicommask.h"

namespace udg {

class DicomMask;

/** Classe que rep la petició del RIS a través d'un socket i la processa per acabar fent un signal indicant quin ha estat la petició del thread
 *
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class ProcessRisRequestThread : public QThread
{
Q_OBJECT
public:

    ProcessRisRequestThread(QObject *parent = 0);

    ///Llegiex a través del socket la petició del RIS i la processa.
    void process(QTcpSocket *qTcpSocket);

signals:

    ///Signal que indica que s'ha fet una petició per descarregar un estudi
    void requestRetrieveStudy(DicomMask mask);

private :

    QTcpSocket *m_qTcpSocket;

    void run();
    void processRequest(QString request);
};

}

#endif
