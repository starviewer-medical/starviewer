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

#ifndef UDGLOCALDATABASESERIESDAL_H
#define UDGLOCALDATABASESERIESDAL_H

#include <QList>

#include "localdatabasebasedal.h"
#include "series.h"

namespace udg {

class DicomMask;
class Series;

/**
    Classe que conté els mètodes d'accés a la Taula series
  */
class LocalDatabaseSeriesDAL : public LocalDatabaseBaseDAL {
public:
    LocalDatabaseSeriesDAL(DatabaseConnection *dbConnection);

    /// Insereix la nova sèrie
    void insert(Series *newSeries);

    /// Updata la nova serie
    void update(Series *seriesToUpdate);

    /// Esborra les sèries que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID i el SeriesUID
    void del(const DicomMask &seriesMaskToDelete);

    /// Cerca les sèries que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID i el SeriesUID
    QList<Series*> query(const DicomMask &seriesMaskToQuery);

private:
    /// Construeix la sentència sql per inserir la nova sèrie
    QString buildSqlInsert(Series *newSeries);

    /// Construeix la sentència updata la sèrie
    QString buildSqlUpdate(Series *seriesToUpdate);

    /// Construeix la setència per fer select de sèries a partir de la màscara, només té en compte el StudyUID, i SeriesUID
    QString buildSqlSelect(const DicomMask &seriesMaskToSelect);

    /// Construeix la setència per esborrar sèries a partir de la màscara, només té en compte el StudyUID, i SeriesUID
    QString buildSqlDelete(const DicomMask &seriesMaskToDelete);

    /// Construeix la sentència del where tenint en compte la màscara, només té en compte el StudyUID, i SeriesUID
    QString buildWhereSentence(const DicomMask &seriesMask);

    /// Emplena un l'objecte series de la fila passada per paràmetre
    Series* fillSeries(char **reply, int row, int columns);
};

}

#endif
