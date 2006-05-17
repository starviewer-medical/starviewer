/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "operation.h"
#include "const.h"

namespace udg {

Operation::Operation()
{
    m_priority = 9999999;
    m_operation = operationUnknow;
}

bool Operation::operator < ( Operation ope ) const 
{
    //ordena al reves, perque la prioritat més gran és la 0    
    if ( ope.getPriority() < m_priority )
    {
        return true;
    }
    else return false;
}    

void Operation::setStudyMask( StudyMask mask )
{
    m_mask = mask;
}

void Operation::setPriority( int priority )
{
     if ( priority != operationPriorityHigh &&
          priority != operationPriorityMedium &&
          priority != operationPriorityLow )
     {
         m_priority = operationPriorityLow;
     }
     else m_priority = priority;
}

void Operation::setOperation( int operation )
{

    if ( operation > operationView )
    {
        m_operation = operationUnknow;
    }
    else m_operation = operation;
    
    // si no s'especifica la prioritat, el setOperation automaticament l'assigna en funció del tipus d'operacio
    if  ( m_priority > operationPriorityLow )
    {    
        if ( operation == operationView )
        {
            m_priority = operationPriorityHigh;
        }
        else m_priority = operationPriorityMedium; 
    }
}

void Operation::setPacsParameters( PacsParameters parameters )
{
    m_pacsParameters = parameters;
}

StudyMask Operation::getStudyMask()
{
    return m_mask;
}

int Operation::getPriority()
{
    return m_priority;
}

int Operation::getOperation()
{
    return m_operation;
}

PacsParameters Operation::getPacsParameters()
{
    return m_pacsParameters;
}

Operation::~Operation()
{
}

}
