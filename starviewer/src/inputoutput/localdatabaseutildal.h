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

#ifndef UDGLOCALDATABASEUTILDAL_H
#define UDGLOCALDATABASEUTILDAL_H

#include "localdatabasebasedal.h"

namespace udg {

/**
 * @brief The LocalDatabaseUtilDAL class contains several methods to interact with the database not related to any specific class.
 */
class LocalDatabaseUtilDAL : public LocalDatabaseBaseDAL {

public:
    LocalDatabaseUtilDAL(DatabaseConnection &databaseConnection);

    /// Compacts the database. Returns true if successful and false otherwise.
    bool compact();

    /// Returns the database revision. If the database revision cannot be determined, returns -1.
    int getDatabaseRevision();

    /// Updates the database revision. Returns true if successful and false otherwise.
    bool updateDatabaseRevision(int databaseRevision);

};

}

#endif
