/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQUEUEOPERATIONLIST_H
#define UDGQUEUEOPERATIONLIST_H

#include <QObject>
#include "operation.h"

namespace udg {

/** Cua amb priortiat que guarda les operacions pendents de ser realizades, aquesta és una classe singletton, 
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QueueOperationList : public QObject
{
Q_OBJECT
    
public:
    
    /** Retorna una instancia de l'objecte
      *        @return instancia de l'objecte
      */
     static QueueOperationList* getQueueOperationList();
    
     /** Insereix una iperacio a la cua
       *        @param operacio que s'insereix
       */
     void insertOperation(Operation ope);
     
     /** Retorna la pròxima operacio que s'ha de realitzar, les ordena per prioritat i ordre d'entrada a la cua. Dins les   * operacions de la mateixa prioritat  sempre agafa que porta més temps a la cua. Al fer el get, esborra l'operacio 
       * que retorna de la llista
       *         @return operacio que té més prioritat dins la cua
       */
     Operation getMaximumPriorityOperation();
     
     /** Indica si la cua està buida
       *        @return boolea que indica si la cua esta buida
       */
     bool isEmpty();
    
    /** Desctructor de la classe
      */
    ~QueueOperationList();

private :

    static QueueOperationList *pInstance;
    QList <Operation> m_queueOperationList;
    
    /** Constructor de la classe
      */
    QueueOperationList(QObject *parent = 0);

};

}

#endif
