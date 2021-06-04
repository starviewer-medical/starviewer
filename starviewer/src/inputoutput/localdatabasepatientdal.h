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

#ifndef UDGLOCALDATABASEPATIENTDAL_H
#define UDGLOCALDATABASEPATIENTDAL_H

#include "localdatabasebasedal.h"

namespace udg {

class DicomMask;
class Patient;

/**
 * @brief The LocalDatabasePatientDAL class is the Data Access Layer class for patients.
 */
class LocalDatabasePatientDAL : public LocalDatabaseBaseDAL {

public:
    LocalDatabasePatientDAL(DatabaseConnection &databaseConnection);

    /// Inserts to the database the given patient. Returns true if successful and false otherwise.
    bool insert(Patient *patient);

    /// Updates in the database the given patient. Returns true if successful and false otherwise.
    bool update(const Patient *patient);

    /// Deletes from the database the patient with the given id. Returns true if successful and false otherwise.
    bool del(qlonglong patientID);

    /// Retrieves from the database the patients that match the given mask (only PatientId is considered) and returns them in a list.
    QList<Patient*> query(const DicomMask &mask);

};

}

#endif
