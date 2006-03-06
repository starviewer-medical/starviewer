/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

 
#include "qinputparameters.h"

namespace udg {

QInputParameters::QInputParameters(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
    m_individualSincronization = true;
}


QInputParameters::~QInputParameters()
{
}


void QInputParameters::disableIndividualSincronization()
{
    m_individualSincronization = false;
}

void QInputParameters::enableIndividualSincronization()
{
    m_individualSincronization = true;
}

}; //end namespace udg
