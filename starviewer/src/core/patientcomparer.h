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

#ifndef PATIENTCOMPARER_H
#define PATIENTCOMPARER_H

#include <QHash>

namespace udg {

class Patient;

/**
    This class has a method to determine if two patients are the same. In case this can't be determined for a given pair of patients, this is asked to the user
    and the choice is remembered for future queries.
 */
class PatientComparer {

public:

    /// Returns true if both patients have to be considered the same patient.
    /// This is determined by comparing their IDs and their names, and in case of doubt the user is asked.
    bool areSamePatient(const Patient *patient1, const Patient *patient2);

private:

    /// Returns true if the user considers both patients to be the same.
    virtual bool askUserIfAreSamePatient(const Patient *patient1, const Patient *patient2) const;

private:

    /// Records user choice for each pair of patients (represented by ID+Name) that can't be distinguished automatically.
    QHash<QString, bool> m_patientsSimilarityUserDecision;

};

} // namespace udg

#endif // PATIENTCOMPARER_H
