#include "keyimagenotemanager.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomreferencedimage.h"
#include "keyimagenote.h"

#include <QDateTime>

namespace udg {

KeyImageNoteManager::KeyImageNoteManager(Patient *patient)
{
    m_patient = patient;
    m_KeyImageNotesOfPatientSearched = false;
}

KeyImageNoteManager::~KeyImageNoteManager()
{
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
                    m_KeyImageNotesOfPatient.append(keyImageNote);
                }
            }
        }
    }
}

QList<KeyImageNote*> KeyImageNoteManager::getKeyImageNotesOfPatient()
{
    if (!m_KeyImageNotesOfPatientSearched)
    {
        m_KeyImageNotesOfPatientSearched = true;
        searchKeyImageNotes();
    }

    return m_KeyImageNotesOfPatient;
}

void KeyImageNoteManager::addImageToTheCurrentSelectionOfImages(Image *image)
{
    if (!m_currentSelection.contains(image))
    {
        m_currentSelection.append(image);
        emit imageAddedToTheCurrentSelectionOfImages(image);
    }
}

void KeyImageNoteManager::createKeyImageNote(const QString &documentTitle, const QString &documentTitleQualityReasons, const QString &observerName, const QString &keyObjectDescription)
{
    // TODO De moment nomes es recullen les dades i es crea un Key Image Note, caldra capturar on es volen guardar i guardar-ho, pendent d'implementacio
    KeyImageNote *newKeyImageNote = new KeyImageNote();

    newKeyImageNote->setDocumentTitle(KeyImageNote::getDocumentTitleInstanceFromString(documentTitle));

    if (KeyImageNote::documentTitleNeedRejectedForQualityReasons(newKeyImageNote->getDocumentTitle()))
    {
        newKeyImageNote->setRejectedForQualityReasons(KeyImageNote::getRejectedForQualityReasonsInstanceFromString(documentTitleQualityReasons));
    }
    else
    {
        newKeyImageNote->setRejectedForQualityReasons(KeyImageNote::NoneRejectedForQualityReasons); 
    }

    newKeyImageNote->setObserverContextType(KeyImageNote::Person);
    newKeyImageNote->setObserverContextName(observerName);
    newKeyImageNote->setKeyObjectDescription(keyObjectDescription);
    newKeyImageNote->setContentDate(QDate::currentDate());
    newKeyImageNote->setContentTime(QTime::currentTime());

    QList<DICOMReferencedImage*> referencedImages;
    foreach (const Image *image, m_currentSelection)
    {
        // TODO Falta assignar UID del pare
        DICOMReferencedImage *referencedImage = new DICOMReferencedImage();
        referencedImage->setDICOMReferencedImageSOPInstanceUID(image->getSOPInstanceUID());
        referencedImage->setFrameNumber(image->getFrameNumber());
    }

    m_currentSelection.clear();
    emit currentSelectionCleared();
}

}