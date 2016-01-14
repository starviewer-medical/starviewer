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

#include "localdatabasebasedal.h"

namespace udg {

class DicomMask;
class Series;

/**
 * @brief The LocalDatabaseSeriesDAL class is the Data Access Layer class for series.
 */
class LocalDatabaseSeriesDAL : public LocalDatabaseBaseDAL {

public:
    LocalDatabaseSeriesDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given series. Returns true if successful and false otherwise.
    bool insert(const Series *series);

    /// Updates in the database the given series. Returns true if successful and false otherwise.
    bool update(const Series *series);

    /// Deletes from the database the series that match the given mask (only StudyUID and SeriesUID are considered).
    /// Returns true if successful and false otherwise.
    bool del(const DicomMask &mask);

    /// Retrieves from the database the series that match the given mask (only StudyUID and SeriesUID are considered) and returns them in a list.
    QList<Series*> query(const DicomMask &mask);

    /// Returns how many series in the database match the given mask (only StudyUID and SeriesUID are considered). Returns -1 in case of error.
    int count(const DicomMask &mask);

private:
    /// Creates and returns a series with the information of the current row of the given query.
    static Series* getSeries(const QSqlQuery &query);

};

}

#endif
