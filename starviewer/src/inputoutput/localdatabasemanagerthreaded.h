/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGLOCALDATABASEMANAGERTHREADED_H
#define UDGLOCALDATABASEMANAGERTHREADED_H

#include <QThread>
#include "localdatabasemanager.h"

namespace udg {

//class LocalDatabaseManager;
class Patient;
    
/** Manager de la base de dades local, permet interactuar amb tots els objectes de la base de dades. Totes les operacions 
    s'executen en un nou thread. 
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class LocalDatabaseManagerThreaded : public QThread
{
Q_OBJECT
    
public:

    /// Tipus d'operació que serà utilitzat per saber quina operació ha finalitzat.
    enum OperationType{Insert, Update, Detete};
    
    ///Constructor de la classe
    LocalDatabaseManagerThreaded(QObject *parent = 0);

    ~LocalDatabaseManagerThreaded();

    ///Retorna l'ultim error produït en la operació de insert
    LocalDatabaseManager::LastError getLastError();

protected:

    ///Mètode que engegar l'event loop en un nou thread.
    void run();

public slots:
    
    ///Inseriex un nou pacient a la base de dades
    void insert(Patient *newPatient);

signals:

    ///S'emetrà quan una operació hagi finalitzat. L'OperationType ens permet saber el tipus d'operació que ha finalitzat.
    void operationFinished(LocalDatabaseManagerThreaded::OperationType);
    
private:
    
    ///Atribut que s'utilitza per anar executant operacions de la bases de dades.
    LocalDatabaseManager m_localDatabaseManager;

};

}

#endif