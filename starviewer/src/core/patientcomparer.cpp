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

#include "patientcomparer.h"

#include "patient.h"
#include "starviewerapplication.h"

#include <QMessageBox>

namespace udg {

bool PatientComparer::areSamePatient(const Patient *patient1, const Patient *patient2)
{
    if (!patient1 || !patient2)
    {
        return false;
    }

    Patient::PatientsSimilarity patientsSimilarity = patient1->compareTo(patient2);

    if (patientsSimilarity == Patient::SamePatients)
    {
        return true;
    }
    else if (patientsSimilarity == Patient::SamePatientIDsDifferentPatientNames || patientsSimilarity == Patient::SamePatientNamesDifferentPatientIDs)
    {
        QString idNamePatient1 = QString("%1_%2").arg(patient1->getID()).arg(patient1->getFullName());
        QString idNamePatient2 = QString("%1_%2").arg(patient2->getID()).arg(patient2->getFullName());
        QString hashKey;

        if (QString::compare(idNamePatient1, idNamePatient2) > 0)
        {
            hashKey = QString("%1_%2").arg(idNamePatient1).arg(idNamePatient2);
        }
        else
        {
            hashKey = QString("%1_%2").arg(idNamePatient2).arg(idNamePatient1);
        }

        if (m_patientsSimilarityUserDecision.contains(hashKey))
        {
            return m_patientsSimilarityUserDecision.value(hashKey);
        }
        else
        {
            bool userDecision = askUserIfAreSamePatient(patient1, patient2);
            m_patientsSimilarityUserDecision.insert(hashKey, userDecision);
            return userDecision;
        }
    }
    else
    {
        return false;
    }
}

bool PatientComparer::askUserIfAreSamePatient(const Patient *patient1, const Patient *patient2) const
{
    QString text = QObject::tr("Cannot determine the similarity of these patients.");
    text += "\n\n";
    text += QObject::tr("Do you consider they are the same?");
    text += "\n\n";
    text += QObject::tr("Patient 1");
    text += "\n";
    text += QObject::tr("ID: %1").arg(patient1->getID());
    text += "\n";
    text += QObject::tr("Name: %1").arg(patient1->getFullName());
    text += "\n\n";
    text += QObject::tr("Patient 2");
    text += "\n";
    text += QObject::tr("ID: %1").arg(patient2->getID());
    text += "\n";
    text += QObject::tr("Name: %1").arg(patient2->getFullName());
    
    return QMessageBox::question(0, ApplicationNameString, text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}

} // namespace udg
