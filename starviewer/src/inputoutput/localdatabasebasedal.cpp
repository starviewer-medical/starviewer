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

#include <QSqlError>
#include <QString>
#include <QVariant>

#include "databaseconnection.h"
#include "logging.h"

namespace udg {

LocalDatabaseBaseDAL::LocalDatabaseBaseDAL(DatabaseConnection *dbConnection)
{
    m_dbConnection = dbConnection;
}

QSqlError LocalDatabaseBaseDAL::getLastError()
{
    return m_dbConnection->getLastError();
}

QString LocalDatabaseBaseDAL::formatTextToValidSQLSyntax(QString string)
{
    return string.isNull() ? "" : string.replace("'", "''");
}

QString LocalDatabaseBaseDAL::formatTextToValidSQLSyntax(QChar qchar)
{
    // Retornem un QString perquè si retornem QChar('') si qchar és null al converti-lo a QString(QChar('')) el QString s'inicialitza incorrectament agafant
    // com a valor un caràcter estrany en comptes de QString("")
    return qchar.isNull() ? "" : QString(qchar);
}

QString LocalDatabaseBaseDAL::convertToQString(const QVariant &text)
{
    QString string = text.toString();

    if (string.contains(QChar::ReplacementCharacter))
    {
        string = QString::fromLatin1(string.toUtf8().constData());
    }

    return string;
}

void LocalDatabaseBaseDAL::logError(const QString &sqlSentence)
{
    #define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */

    // Ingnorem l'error de clau duplicada
    if (getLastError().nativeErrorCode().toInt() != SQLITE_CONSTRAINT)
    {
        ERROR_LOG("S'ha produit l'error: " + getLastError().nativeErrorCode() + ", " + m_dbConnection->getLastErrorMessage() +
                  ", al executar la seguent sentencia sql " + sqlSentence);
    }
}

}
