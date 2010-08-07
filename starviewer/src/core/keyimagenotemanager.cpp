#include "keyimagenotemanager.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomreferencedimage.h"
#include "keyimagenote.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "volume.h"

#include <dcmtk/dcmdata/dcuid.h>
#include <dcfilefo.h> // Generar DICOM
#include <dsrdoc.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/dcmdata/dcdebug.h>
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/ofstd/ofconapp.h>

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
            
            if (storeToPacs) // GENERAR DICOMS
            {
                generateKeyImageNoteDICOMFile(newKeyImageNoteSeries);
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

QList<KeyImageNote*> KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(Patient *patient, Image *image)
{
    QList <KeyImageNote*> keyImageNotesOfImage;

    foreach (Study *study, patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            if (series->getModality() == "KO")
            {
                foreach (KeyImageNote *keyImageNote, series->getKeyImageNotes())
                {
                    foreach(DICOMReferencedImage *referencedImage, keyImageNote->getDICOMReferencedImages())
                    {
                        if (image->getSOPInstanceUID() == referencedImage->getDICOMReferencedImageSOPInstanceUID() && image->getFrameNumber() == referencedImage->getFrameNumber())
                        {
                            keyImageNotesOfImage.append(keyImageNote);
                        }
                    }
                }
            }
        }
    }

    return keyImageNotesOfImage;
}

void KeyImageNoteManager::changeCurrentDisplayedImage(const QString &imageInstanceUID)
{
    Volume *volumeOfImage = getVolumeWhereImageIsReferenced(imageInstanceUID);
    
    if (volumeOfImage)
    {   
        int sliceOfImage;
        foreach (Image *currentImage, volumeOfImage->getImages())
        {
            if (currentImage->getSOPInstanceUID() == imageInstanceUID)
            {
                sliceOfImage = currentImage->getOrderNumberInVolume();
                break;
            }
        }

        emit changeCurrentSlice(volumeOfImage, sliceOfImage);
    }
}

Volume* KeyImageNoteManager::getVolumeWhereImageIsReferenced(const QString &sopInstanceUID)
{
    foreach (Study *study, m_patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            foreach (Volume *currentVolume, series->getVolumesList())
            {
                foreach (Image *currentImage, currentVolume->getImages())
                {
                    if (currentImage->getSOPInstanceUID() == sopInstanceUID)
                    {
                        return currentVolume;
                    }
                }
            }
        }
    }

    return NULL;
}

void KeyImageNoteManager::showKeyImageNote(KeyImageNote *keyImageNote)
{
    emit showImagesReferencedInKeyImageNote(keyImageNote->getReferencedImages());
}

void KeyImageNoteManager::removeItemOfCurrentSelection(QString removedUID)
{
    // TODO: Falta considerar el numero de frame
    bool found = false;
    int i = 0;
    while (!found && i < m_currentSelection.size())
    {
        if (m_currentSelection.at(i)->getSOPInstanceUID() == removedUID)
        {
            m_currentSelection.removeAt(i);
            found = true;
        }
        
        i++;
    }

}

void KeyImageNoteManager::generateKeyImageNoteDICOMFile(Series *newKeyImageNoteSeries)
{
    DSRDocument *keyImageNoteDocument = new DSRDocument(DSRTypes::DT_KeyObjectDoc);
    KeyImageNote *keyImageNote = newKeyImageNoteSeries->getKeyImageNotes().at(0);
    
    // Emplenem dades del modul de Patient
    keyImageNoteDocument->setPatientID(qPrintable(newKeyImageNoteSeries->getParentStudy()->getParentPatient()->getID()));
    keyImageNoteDocument->setPatientsName(qPrintable(newKeyImageNoteSeries->getParentStudy()->getParentPatient()->getFullName()));
    keyImageNoteDocument->setPatientsBirthDate(qPrintable(newKeyImageNoteSeries->getParentStudy()->getParentPatient()->getBirthDate().toString("yyyyMMdd")));
    keyImageNoteDocument->setPatientsSex(qPrintable(newKeyImageNoteSeries->getParentStudy()->getParentPatient()->getSex()));

    // Emplenem dades del modul de Study
    keyImageNoteDocument->createNewSeriesInStudy(qPrintable(newKeyImageNoteSeries->getParentStudy()->getInstanceUID()));
    keyImageNoteDocument->setAccessionNumber(qPrintable(newKeyImageNoteSeries->getParentStudy()->getAccessionNumber()));
    keyImageNoteDocument->setStudyDescription(qPrintable(newKeyImageNoteSeries->getParentStudy()->getDescription()));
    keyImageNoteDocument->setStudyID(qPrintable(newKeyImageNoteSeries->getParentStudy()->getID()));

    // Emplenem dades del modul SOP Common
    keyImageNoteDocument->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

    // Generem l'arbre

    // Document Title
    keyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    keyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue(qPrintable(QString::number(keyImageNote->getDocumentTitle())), "DCM", qPrintable(keyImageNote->getDocumentTitleAsString(keyImageNote->getDocumentTitle()))));

    // Observer Type
    keyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
    keyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("121005", "DCM", "Observer Type"));
    keyImageNoteDocument->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("121006", "DCM", "Person"));

    keyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName);
    keyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("121008", "DCM", "Person Observer Name"));
    keyImageNoteDocument->getTree().getCurrentContentItem().setStringValue(qPrintable(keyImageNote->getObserverContextName()));
    
    // Description
    keyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    keyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("113012", "DCM", "Key Object Description"));
    keyImageNoteDocument->getTree().getCurrentContentItem().setStringValue(qPrintable(keyImageNote->getKeyObjectDescription()));

    // Referenced Images
    foreach (Image *image, keyImageNote->getReferencedImages())
    {
        keyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
        keyImageNoteDocument->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("1.2.840.10008.5.1.4.1.1.2", qPrintable(image->getSOPInstanceUID())));
        keyImageNoteDocument->getCurrentRequestedProcedureEvidence().addItem(keyImageNoteDocument->getStudyInstanceUID(), keyImageNoteDocument->getSeriesInstanceUID(), "1.2.840.10008.5.1.4.1.1.2", qPrintable(image->getSOPInstanceUID()));
    }
    
    DcmFileFormat *fileformat = new DcmFileFormat();
    
    OFCondition status = keyImageNoteDocument->write(*fileformat->getDataset());
    
    if (status.good())
    {
        OFString filename = "provaDICOM.dcm";
        fileformat->saveFile(filename.c_str(), EXS_LittleEndianExplicit);
    }
    else
    {
        DEBUG_LOG(QString("S'ha produit un error al escriure el DSRDocument: " + QString(status.text())));
    }
}
}