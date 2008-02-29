/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "queueoperationlist.h"
#include <QtAlgorithms>

namespace udg {

QueueOperationList::QueueOperationList(QObject *parent)
 : QObject(parent)
{
}

QueueOperationList* QueueOperationList::pInstance = 0;

QueueOperationList * QueueOperationList::getQueueOperationList()
{
    if (pInstance == 0)
    {
        pInstance = new QueueOperationList;
    }

    return pInstance;
}


void QueueOperationList::insertOperation(Operation operation )
{
    bool ok = true;
    foreach( Operation operationItem, m_queueOperationList )
    {
        if( operation == operationItem )
        {
            ok = false;
            break;
        }
    }
    if( ok )
        m_queueOperationList << operation;
}

Operation QueueOperationList::getMaximumPriorityOperation()
{
    //s'ordena la llista
    Operation ope;
    int maxPriorityOperation = 1000, positionMaxPriorityOperation = 0;

    //s'agafa la primera operacio i l'esborra
    for (int i = 0;i<m_queueOperationList.count();i++)
    {
        ope = m_queueOperationList.at(i);

        if (ope.getPriority() < maxPriorityOperation)
        {
            maxPriorityOperation = ope.getPriority();
            positionMaxPriorityOperation = i;
        }
    }

    if (!m_queueOperationList.isEmpty())
    {
        ope = m_queueOperationList.at(positionMaxPriorityOperation);
        m_queueOperationList.removeAt(positionMaxPriorityOperation);
    }

    return ope;

}


bool QueueOperationList::isEmpty()
{
    return m_queueOperationList.isEmpty();
}

QueueOperationList::~QueueOperationList()
{
}


}
