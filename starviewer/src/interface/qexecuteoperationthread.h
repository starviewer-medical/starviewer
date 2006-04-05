/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQEXECUTEOPERATIONTHREAD_H
#define UDGQEXECUTEOPERATIONTHREAD_H

#include <QThread>
#include "../interface/qretrievescreen.h"


namespace udg {

/** Aquest classe, s'encarrega d'anar executant objectes Operation. (operacions que s'han de dur a terme). Aquesta classe crea un thread quan hi ha alguna operacio i les executa. A més també utilitza una cua, on es van guardant totes les operation pendents d'executar 
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Operation;

class QExecuteOperationThread :public QThread
{
Q_OBJECT

public:
    /** Constructor de la classe
      */
    QExecuteOperationThread( QObject *parent = 0 );

    /** afegeix una operacio a la cua, si la cua esta buida l'afegeix i crea el thread que l'executarà 
      *         @param Operation a executar
      */
    void queueOperation( Operation operation );
    
    /** Codi que s'executa en un nou thread, evalua l'operacio i decideix que fer
      */
    void run();
    
    /** Destructor de la classe
      */
    ~QExecuteOperationThread();

signals :
    void viewStudy( QString studyUID );
    
    void setStudyRetrieving( QString studyUID );
    void setStudyRetrieved( QString studyUID );
    void setErrorRetrieving( QString studyUID );

private slots :

    /** Emet un signal perquè es visualitzi una seria de l'estudi amb l'UID passat per parametre
      *     @param studyUID de l'estudi a descarregar
      */
    void seriesRetrieved( QString );

private :
    
    QRetrieveScreen * m_qretrieveScreen;
    bool m_stop;//indica si el thread esta parat
        
    /** Crea connexions entre el QRetrieveScreen i aquesta classe
      */
    void createConnections();
    /** Descarrega un estudi, segons els paràmetres a operation, si l'estudi s'ha de visualitzar 
      * captura els signals de l'objecte starviewersettings que es emes cada vegada que finalitza la descarrega d'una  serie
      *     @param operation a executar
      *     @param indica si l'estudi s'ha de visualitzar
      */
    void retrieveStudy(Operation operation,bool view);
    

};

}

#endif
