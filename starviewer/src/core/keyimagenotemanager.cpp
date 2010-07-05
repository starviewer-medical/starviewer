#include "keyimagenotemanager.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "keyimagenote.h"
#include "keyimagenotemanagerwidget.h"

namespace udg {

KeyImageNoteManager::KeyImageNoteManager(Patient *patient)
{
    m_patient = patient;
    initialize();
}

KeyImageNoteManager::~KeyImageNoteManager()
{
}

void KeyImageNoteManager::initialize()
{
   searchKeyImageNotes();
}

void KeyImageNoteManager::searchKeyImageNotes()
{
    foreach (Study *study, m_patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            if (series->getModality() == "KO")
            {
                foreach (KeyImageNote *keyImageNote, series->getKeyImageNotes())
                {
                    m_kinSet.append(keyImageNote);
                }
            }
        }
    }
}

QList<KeyImageNote*> KeyImageNoteManager::getKeyImageNotes() const
{
    return m_kinSet;
}

}