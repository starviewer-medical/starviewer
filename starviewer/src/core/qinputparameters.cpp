/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#include "qinputparameters.h"

namespace udg {

QInputParameters::QInputParameters( QWidget *parent )
 : QWidget( parent )
{
    m_individualSynchronization = true;
}

QInputParameters::~QInputParameters()
{
}

void QInputParameters::disableIndividualSynchronization()
{
    m_individualSynchronization = false;
}

void QInputParameters::enableIndividualSynchronization()
{
    m_individualSynchronization = true;
}

}; //end namespace udg
