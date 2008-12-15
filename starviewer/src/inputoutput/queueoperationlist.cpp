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
    if (!m_queueOperationList.contains(operation))
    {
        m_queueOperationList << operation;
    }
}

Operation QueueOperationList::takeMaximumPriorityOperation()
{
    Operation operationMaxPriority, operationAtIndex;
    int positionMaxPriorityOperation = 0;

    if (!m_queueOperationList.isEmpty())
    {
        operationMaxPriority = m_queueOperationList.at(positionMaxPriorityOperation);

        for (int index = 1; index < m_queueOperationList.count(); index++)
        {
            operationAtIndex = m_queueOperationList.at(index);

            if (operationAtIndex.getPriority() < operationMaxPriority.getPriority())
            {
                //la operació és més prioritària
                operationMaxPriority = operationAtIndex;
                positionMaxPriorityOperation = index;
            }
        }

        m_queueOperationList.removeAt(positionMaxPriorityOperation);//treiem l'operació de màxim prioritat de la llista
    }

    return operationMaxPriority;
}


bool QueueOperationList::isEmpty()
{
    return m_queueOperationList.isEmpty();
}

QueueOperationList::~QueueOperationList()
{
}


}
