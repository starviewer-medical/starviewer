/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabasemanager.h"

#include "patient.h"

namespace udg
{

LocalDatabaseManager::LocalDatabaseManager()
{
}


Status LocalDatabaseManager::insert(Patient *newPatient)
{
    Status state;
    state.setStatus("Normal", true, 0);

    return state;
}

}
