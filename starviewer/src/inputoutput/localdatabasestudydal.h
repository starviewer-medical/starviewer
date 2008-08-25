/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLOCALDATABASESTUDY_H
#define UDGLOCALDATABASESTUDY_H

#include <QList>

#include "study.h"

class QDate;

namespace udg {

class DatabaseConnection;
class DicomMask;

/** Aquesta classe s'encarrega de dur a terme les operacions relacionades amb l'objecte estudi de la cache de l'aplicació.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseStudyDAL
{
public:

    LocalDatabaseStudyDAL();

    ///insereix el nou estudi, i insereix com LastAccessDate la data actual
    void insert(Study *newStudy, QDate lastAccessData);

    ///updata l'estudi 
    void update(Study *studyToUpdate, QDate LastAcessDate);

    ///Esborra els estudis que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID
    void del(DicomMask studyMaskToDelete);

    ///Cerca les sèries que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID i el SeriesUID
    QList<Study*> query(DicomMask studyMaskToQuery);

    ///Connexió de la base de dades a utilitzar
    void setConnection(DatabaseConnection *dbConnection);

    ///Retorna l'estat de la última operació realitzada
    int getLastError();

private :

    DatabaseConnection *m_dbConnection;

    int m_lastSqliteError;

    ///Construeix la sentència sql per inserir la nova sèrie
    QString buildSqlInsert(Study *newStudy, QDate lastAcessDate);

    ///Construeix la sentència updata la sèrie
    QString buildSqlUpdate(Study *studyToUpdate, QDate lastAccessDate);

    ///Construeix la setència per fer select de sèries a partir de la màscara, només té en compte el StudyUID i els estudis que tinguin un LastAccessDate menor que el de la màscara
    QString buildSqlSelect(DicomMask studyMaskToSelect);

    ///Construeix la setència per esborrar sèries a partir de la màscara, només té en compte el StudyUID
    QString buildSqlDelete(DicomMask studyMaskToDelete);

    ///Emplena un l'objecte series de la fila passada per paràmetre
    Study* fillStudy(char **reply, int row, int columns);

    ///Ens fa un ErrorLog d'una sentència sql
    void logError(QString sqlSentence);

};
}

#endif
