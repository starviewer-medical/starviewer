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

#include "localdatabasebasedal.h"

#include <QString>
#include <sqlite3.h>

#include "databaseconnection.h"
#include "logging.h"

namespace udg {

LocalDatabaseBaseDAL::LocalDatabaseBaseDAL(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
    m_lastSqliteError = SQLITE_OK;
}

int LocalDatabaseBaseDAL::getLastError()
{
    return m_lastSqliteError;
}

QString LocalDatabaseBaseDAL::convertToQString(const char *text)
{
    QString string = QString::fromUtf8(text);

    if (string.contains(QChar::ReplacementCharacter))
    {
        string = QString::fromLatin1(text);
    }

    return string;
}

void LocalDatabaseBaseDAL::logError(const QString &sqlSentence)
{
    // Ingnorem l'error de clau duplicada
    if (getLastError() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produit l'error: " + QString().setNum(getLastError()) + ", " + m_dbConnection->getLastErrorMessage() +
                  ", al executar la seguent sentencia sql " + sqlSentence);
    }
}

}
