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

#ifndef LOCALDATABASEPACSDEVICEDAL_H
#define LOCALDATABASEPACSDEVICEDAL_H

#include "localdatabasebasedal.h"

namespace udg {

class PacsDevice;

/**
 * @brief The LocalDatabasePACSRetrievedImagesDAL class is the Data Access Layer class for PACS devices (PACSRetrievedImages table in the database).
 */
class LocalDatabasePACSRetrievedImagesDAL : public LocalDatabaseBaseDAL {

public:
    LocalDatabasePACSRetrievedImagesDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given PACS device. Returns the id of the inserted row if successful and -1 otherwise.
    qlonglong insert(const PacsDevice &pacsDevice);

    /// Returns the PACS with the given id in the database. If there's no PACS with the given id, returns a default PacsDevice.
    PacsDevice query(qlonglong pacsId);

    /// Returns the id of the given PACS in the database. If this PACS is not in the database, returns a null QVariant.
    QVariant queryId(const PacsDevice &pacsDevice);
};

}

#endif // LOCALDATABASEPACSDEVICEDAL_H
