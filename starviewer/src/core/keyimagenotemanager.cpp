#include "keyimagenotemanager.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomreferencedimage.h"
#include "keyimagenote.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include <dcmtk/dcmdata/dcuid.h>

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

void KeyImageNoteManager::generateAndStoreNewKeyImageNote(const QString &documentTitle, const QString &documentTitleQualityReasons, const QString &observerName, const QString &keyObjectDescription, bool storeToLocalDataBase, bool storeToPacs, const QString &pacsNode)
{
    if (m_currentSelection.count() > 0)
    {
       if (allImagesInTheSameStudy())
       {
            Series *newKeyImageNoteSeries = createNewKeyImageNoteSeries();
            KeyImageNote *newKeyImageNote = createNewKeyImageNote(documentTitle, documentTitleQualityReasons, observerName, keyObjectDescription);
            newKeyImageNoteSeries->addKeyImageNote(newKeyImageNote);

            if (storeToLocalDataBase)
            {
                storeKeyImageNoteSeriesToLocalDataBase(newKeyImageNoteSeries);
            }

            m_currentSelection.clear();
            emit currentSelectionCleared();

            m_KeyImageNotesOfPatientSearched = false;
            emit keyImageNoteOfPatientAdded(newKeyImageNote);
       }
       else
       {
           DEBUG_LOG("No totes les imatges son del mateix estudi");
       }
    }
    else
    {
        DEBUG_LOG("Has de seleccionar almenys una imatge");
    }
}

KeyImageNote* KeyImageNoteManager::createNewKeyImageNote(const QString &documentTitle, const QString &documentTitleQualityReasons, const QString &observerName, const QString &keyObjectDescription)
{
    KeyImageNote *newKeyImageNote = new KeyImageNote();

    newKeyImageNote->setDocumentTitle(KeyImageNote::getDocumentTitleInstanceFromString(documentTitle));

    if (KeyImageNote::isDocumentTitleModifiedForQualityReasonsOrIssues(newKeyImageNote->getDocumentTitle()))
    {
        newKeyImageNote->setRejectedForQualityReasons(KeyImageNote::getRejectedForQualityReasonsInstanceFromString(documentTitleQualityReasons));
    }
    else
    {
        newKeyImageNote->setRejectedForQualityReasons(KeyImageNote::NoneRejectedForQualityReasons); 
    }

    newKeyImageNote->setInstanceNumber("1");
    char instanceUid[100];
    dcmGenerateUniqueIdentifier(instanceUid, SITE_INSTANCE_UID_ROOT);
    newKeyImageNote->setInstanceUID(QString(instanceUid));

    newKeyImageNote->setObserverContextType(KeyImageNote::Person);
    newKeyImageNote->setObserverContextName(observerName);
    newKeyImageNote->setKeyObjectDescription(keyObjectDescription);
    newKeyImageNote->setContentDate(QDate::currentDate());
    newKeyImageNote->setContentTime(QTime::currentTime());

    QList<DICOMReferencedImage*> referencedImages;
    foreach (const Image *image, m_currentSelection)
    {
        DICOMReferencedImage *referencedImage = new DICOMReferencedImage();
        referencedImage->setDICOMReferencedImageSOPInstanceUID(image->getSOPInstanceUID());
        referencedImage->setFrameNumber(image->getFrameNumber());
        referencedImage->setReferenceParentSOPInstanceUID(newKeyImageNote->getInstanceUID());
        referencedImages.append(referencedImage);
    }

    newKeyImageNote->setDICOMReferencedImages(referencedImages);

    return newKeyImageNote;
}

Series* KeyImageNoteManager::createNewKeyImageNoteSeries()
{
    Series *newKeyImageNoteSeries = new Series();
    Study *parentStudy = m_currentSelection[0]->getParentSeries()->getParentStudy();

    newKeyImageNoteSeries->setDate(QDate::currentDate());
    newKeyImageNoteSeries->setTime(QTime::currentTime());
    newKeyImageNoteSeries->setSeriesNumber(QString("0000")+QString::number(parentStudy->getSeries().count()));
    newKeyImageNoteSeries->setModality("KO");

    char seriesUid[100];
    dcmGenerateUniqueIdentifier(seriesUid, SITE_INSTANCE_UID_ROOT);
    newKeyImageNoteSeries->setInstanceUID(QString(seriesUid));

    parentStudy->addSeries(newKeyImageNoteSeries);

    return newKeyImageNoteSeries;
}

void KeyImageNoteManager::storeKeyImageNoteSeriesToLocalDataBase(Series *newKeyImageNoteSeries)
{
    LocalDatabaseManager localDataBaseManager;

    localDataBaseManager.save(newKeyImageNoteSeries);
}

bool KeyImageNoteManager::allImagesInTheSameStudy()
{
    Study *parentStudy = m_currentSelection[0]->getParentSeries()->getParentStudy();

    int i = 1;
    while (i < m_currentSelection.size() && parentStudy ==  m_currentSelection[i]->getParentSeries()->getParentStudy() )
    {
        i++;
    }

    return i == m_currentSelection.size();
}

void KeyImageNoteManager::changeCurrentDisplayedImage(const QString &seriesInstanceUID, const QString &imageInstanceUID)
{
    Series * series = m_patient->getSeries(seriesInstanceUID);

    if (series != NULL)
    {
        int i = 0;
        Image *image = NULL;
        while (!image && i < series->getNumberOfImages())
        {
            if (series->getImages().at(i)->getSOPInstanceUID() == imageInstanceUID)
            {
                image = series->getImages().at(i);
            }

            i++;
        }

        if (image != NULL)
        {
            emit changeCurrentSlice(image->getOrderNumberInVolume());
        }
    }
}

void KeyImageNoteManager::showKeyImageNote(KeyImageNote *keyImageNote)
{
    emit showImagesReferencedInKeyImageNote(keyImageNote->getReferencedImages());
}

}