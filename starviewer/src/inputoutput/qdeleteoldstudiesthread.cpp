/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdeleteoldstudiesthread.h"

#include "logging.h"

namespace udg
{

QDeleteOldStudiesThread::QDeleteOldStudiesThread(QObject *parent)
 : QThread(parent)
{
}

void QDeleteOldStudiesThread::deleteOldStudies()
{
    start();
}

LocalDatabaseManager::LastError QDeleteOldStudiesThread::getLastError()
{
    return m_lastError;
}

void QDeleteOldStudiesThread::run()
{
    LocalDatabaseManager localDatabaseManager;

    localDatabaseManager.deleteOldStudies();

    m_lastError = localDatabaseManager.getLastError();

    emit finished();
}

}
