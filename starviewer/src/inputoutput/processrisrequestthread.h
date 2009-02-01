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

    ///Crea el thread per atendre el socket i processa les dades rebudes
    void process(QTcpSocket *qTcpSocket);

signals:

    ///Signal que indica que s'ha fet una petició per descarregar un estudi
    void requestRetrieveStudy(DicomMask mask);

private :

    QTcpSocket *m_qTcpSocket;
    ///Indiquem el temps d'espera per llegir la petició del RIS, sinó arriba en aquest temps fem time out
    static const int msTimeOutToReadData;

    ///Aquest mètode ja s'executa dins el thread, Espera rebre les dades del socket i les processa
    void run();

    ///Processa la cadena rebuda del RIS
    void processRequest(QString request);
};

}

#endif
