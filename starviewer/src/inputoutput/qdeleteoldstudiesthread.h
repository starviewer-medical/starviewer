/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/


#ifndef UDGDELETEOLDSTUDIESTHREAD_H
#define UDGDELETEOLDSTUDIESTHREAD_H

#include <QThread>

#include "localdatabasemanager.h"

namespace udg {

/**
    Classe encarregada d'esborrar en un thread els estudis vells
  */
class QDeleteOldStudiesThread : public QThread {
Q_OBJECT

public:
    /// Constructor de la classe
    QDeleteOldStudiesThread(QObject *parent = 0);

    /// Esborra els estudis vells engegant un thread
    void deleteOldStudies();

    /// Retorna l'estat de l'operació d'esborrar estudis vells
    LocalDatabaseManager::LastError getLastError();

signals:
    /// Signal que s'envia quan finalitza l'execució d'aquest thread
    void finished();

private:
    /// Métode que és excutat pel thread creat per Qt, que esborra els estudis vells
    void run();

    LocalDatabaseManager::LastError m_lastError;

};

}

#endif
