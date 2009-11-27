/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "previousstudiesmanager.h"
#include "study.h"

namespace udg {

PreviousStudiesManager::PreviousStudiesManager()
{
}

PreviousStudiesManager::~PreviousStudiesManager()
{

}

void PreviousStudiesManager::queryPreviousStudies(Study *study)
{
    Q_UNUSED( study );
}

void PreviousStudiesManager::cancelCurrentQuery()
{

}

bool PreviousStudiesManager::isExecutingQueries()
{
    return false;
}

}
