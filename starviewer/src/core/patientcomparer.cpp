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
    QString text = QObject::tr("Cannot determine the similarity of these patients.\n\nDo you consider they are the same?\n\n");
    text += QObject::tr("Patient 1\nID: %1\nName: %2\n\n").arg(patient1->getID()).arg(patient1->getFullName());
    text += QObject::tr("Patient 2\nID: %1\nName: %2").arg(patient2->getID()).arg(patient2->getFullName());
    return QMessageBox::question(0, ApplicationNameString, text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}

} // namespace udg
