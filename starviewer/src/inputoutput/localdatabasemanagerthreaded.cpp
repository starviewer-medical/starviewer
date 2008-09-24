/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabasemanagerthreaded.h"
#include <iostream>

namespace udg
{
    
LocalDatabaseManagerThreaded::LocalDatabaseManagerThreaded(QObject *parent)
: QThread(parent)
{

}    

LocalDatabaseManagerThreaded::~LocalDatabaseManagerThreaded()
{
    if ( isRunning() )//Si s'està executan el thread el matem, perquè estem tancant l'aplicació, sinó el thread queda per sota obert
    {
        terminate();
        wait();//Hem d'esperar que es mati el thread per poder continuar
    }
}
void LocalDatabaseManagerThreaded::insert(Patient *newPatient)
{
    m_localDatabaseManager.insert(newPatient);
    emit operationFinished(Insert);
}
    
void LocalDatabaseManagerThreaded::run() 
{
    exec();
}

}