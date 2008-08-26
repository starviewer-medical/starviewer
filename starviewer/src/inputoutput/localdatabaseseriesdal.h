/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLOCALDATABASESERIESDAL_H
#define UDGLOCALDATABASESERIESDAL_H

#include <QList>

#include "series.h"

namespace udg {

class DicomMask;
class Series;
class DatabaseConnection;

/** Classe que conté els mètodes d'accés a la Taula series
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseSeriesDAL
{

public:
    LocalDatabaseSeriesDAL();

    ///insereix la nova sèrie
    void insert(Series *newSeries);

    ///updata la nova serie
    void update(Series *seriesToUpdate);

    ///Esborra les sèries que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID i el SeriesUID
    void del(DicomMask seriesMaskToDelete);

    ///Cerca les sèries que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID i el SeriesUID
    QList<Series*> query(DicomMask seriesMaskToQuery);

    ///Connexió de la base de dades a utilitzar
    void setDatabaseConnection(DatabaseConnection *dbConnection);

    ///Retorna l'estat de la última operació realitzada
    int getLastError();

private:

    DatabaseConnection *m_dbConnection;

    int m_lastSqliteError;

    ///Construeix la sentència sql per inserir la nova sèrie
    QString buildSqlInsert(Series *newSeries);

    ///Construeix la sentència updata la sèrie
    QString buildSqlUpdate(Series *seriesToUpdate);

    ///Construeix la setència per fer select de sèries a partir de la màscara, només té en compte el StudyUID, i SeriesUID
    QString buildSqlSelect(DicomMask seriesMaskToSelect);

    ///Construeix la setència per esborrar sèries a partir de la màscara, només té en compte el StudyUID, i SeriesUID
    QString buildSqlDelete(DicomMask seriesMaskToDelete);

    ///Construeix la sentència del where tenint en compte la màscara, només té en compte el StudyUID, i SeriesUID
    QString buildWhereSentence(DicomMask seriesMask);

    ///Emplena un l'objecte series de la fila passada per paràmetre
    Series* fillSeries(char **reply, int row, int columns);

    ///Ens fa un ErrorLog d'una sentència sql
    void logError(QString sqlSentence);
};

}

#endif
