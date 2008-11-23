/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGDELETEOLDSTUDIESTHREAD_H
#define UDGDELETEOLDSTUDIESTHREAD_H

#include <QThread>

#include "localdatabasemanager.h"

namespace udg {

/** Classe encarregada d'esborrar en un thread els estudis vells
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QDeleteOldStudiesThread : public QThread
{
Q_OBJECT

public:
    /** Constructor de la classe
      */
    QDeleteOldStudiesThread( QObject *parent = 0 );

    ///Esborra els estudis vells engegant un thread
    void deleteOldStudies();

    ///Retorna l'estat de l'operació d'esborrar estudis vells
    LocalDatabaseManager::LastError getLastError();

signals:

    ///signal que s'envia quan finalitza l'execució d'aquest thread
    void finished();

private:

    ///Métode que és excutat pel thread creat per Qt, que esborra els estudis vells
    void run();

    LocalDatabaseManager::LastError m_lastError;

};

}

#endif
