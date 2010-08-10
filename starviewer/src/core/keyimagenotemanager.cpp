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
#include <dvpshlp.h>
#include <QDateTime>
#include <QDir>

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

KeyImageNote* KeyImageNoteManager::generateAndStoreNewKeyImageNote(const QString &documentTitle, const QString &documentTitleQualityReasons, const QString &observerName, const QString &keyObjectDescription, bool storeToLocalDataBase, bool storeToPacs, const QString &pacsNode)
{
    if (!allImagesInTheSameStudy())
    {
        ERROR_LOG("Totes les imatges han de ser el mateix estudi per a crear un KIN");
        return NULL;
    }
    Series *newKeyImageNoteSeries = createNewKeyImageNoteSeries();
    KeyImageNote *newKeyImageNote = createNewKeyImageNote(documentTitle, documentTitleQualityReasons, observerName, keyObjectDescription);
    newKeyImageNoteSeries->addKeyImageNote(newKeyImageNote);
    
    DSRDocument *keyImageNoteDocument = generateKeyImageNoteDICOMFile(newKeyImageNoteSeries);
    
    if (!storeKeyImageNoteDocumentToDICOMCache(keyImageNoteDocument, newKeyImageNote))
    {
        ERROR_LOG("No s'ha pogut guardar el fitxer DICOM del nou KIN");
        return NULL;
    }
    if (storeToLocalDataBase)
    {
        if (!storeKeyImageNoteSeriesToLocalDataBase(newKeyImageNoteSeries))
        {
            ERROR_LOG("No s'ha pogut guardar a la base de dades local el nou KIN");
            return NULL;
        }
    }
    m_currentSelection.clear();
    emit currentSelectionCleared();
    
    m_KeyImageNotesOfPatientSearched = false;
    emit keyImageNoteOfPatientAdded(newKeyImageNote);

    return newKeyImageNote;
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
        referencedImages.append(referencedImage);
    }

    newKeyImageNote->setDICOMReferencedImages(referencedImages);

    return newKeyImageNote;
}

Series* KeyImageNoteManager::createNewKeyImageNoteSeries()
{
    Series *newKeyImageNoteSeries = new Series();

    newKeyImageNoteSeries->setDate(QDate::currentDate());
    newKeyImageNoteSeries->setTime(QTime::currentTime());
    newKeyImageNoteSeries->setModality("KO");

    return newKeyImageNoteSeries;
}

bool KeyImageNoteManager::storeKeyImageNoteSeriesToLocalDataBase(Series *newKeyImageNoteSeries)
{
    LocalDatabaseManager localDataBaseManager;

    localDataBaseManager.save(newKeyImageNoteSeries);

    return localDataBaseManager.getLastError() == LocalDatabaseManager::Ok;
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

DSRDocument* KeyImageNoteManager::generateKeyImageNoteDICOMFile(Series *newKeyImageNoteSeries)
{
    DSRDocument *keyImageNoteDocument = new DSRDocument(DSRTypes::DT_KeyObjectDoc);
    keyImageNoteDocument->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

    fillStructuredReportCommonData(keyImageNoteDocument, newKeyImageNoteSeries);
    fillKeyObjectSelectionTemplate(keyImageNoteDocument, newKeyImageNoteSeries);
    
    return keyImageNoteDocument;
}

bool KeyImageNoteManager::storeKeyImageNoteDocumentToDICOMCache(DSRDocument *newKeyImageNoteDocument, KeyImageNote *newKeyImageNote)
{
    QString keyImageNotePath;
    DcmFileFormat *fileformat = new DcmFileFormat();

    OFCondition status = newKeyImageNoteDocument->write(*fileformat->getDataset());
    
    if (status.bad())
    {
        DEBUG_LOG("No s'ha escrit correctament al Data Set");
        return false;
    }

    LocalDatabaseManager localDataBaseManager;
    QString path = localDataBaseManager.getStudyPath(QString(newKeyImageNoteDocument->getStudyInstanceUID())) + QDir().separator() + QString(newKeyImageNoteDocument->getSeriesInstanceUID());
    
    if (!QDir().mkpath(path))
    {
        DEBUG_LOG("No s'ha generat correctament el directori per a crear el KIN");
        return false;
    }

    keyImageNotePath = path + QDir().separator() + QString(newKeyImageNoteDocument->getSOPInstanceUID());
    OFCondition saveFileCondition = DVPSHelper::saveFileFormat(qPrintable(keyImageNotePath), fileformat, true);

    if (saveFileCondition.bad())
    {
        DEBUG_LOG(QString("No s'ha guardat correctament el fitxer a %1").arg(path));
        return false;
    }

    newKeyImageNote->setPath(keyImageNotePath);

    return true;
}

void KeyImageNoteManager::fillKeyObjectSelectionTemplate(DSRDocument *newKeyImageNoteDocument, Series *newKeyImageNoteSeries)
{
    KeyImageNote *newKeyImageNote = newKeyImageNoteSeries->getKeyImageNotes().at(0);

    // Document Title
    newKeyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue(qPrintable(QString::number(newKeyImageNote->getDocumentTitle())), "DCM", qPrintable(newKeyImageNote->getDocumentTitleAsString(newKeyImageNote->getDocumentTitle()))));

    // Observer Type
    newKeyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("121005", "DCM", "Observer Type"));
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("121006", "DCM", "Person"));

    newKeyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName);
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("121008", "DCM", "Person Observer Name"));
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setStringValue(qPrintable(newKeyImageNote->getObserverContextName()));
    
    // Description
    newKeyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("113012", "DCM", "Key Object Description"));
    newKeyImageNoteDocument->getTree().getCurrentContentItem().setStringValue(qPrintable(newKeyImageNote->getKeyObjectDescription()));

    // Referenced Images
    foreach (Image *image, newKeyImageNote->getReferencedImages())
    {
        newKeyImageNoteDocument->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);

        // TODO: El SOP Class UID que assignem es un de qualsevol, caldria segons la imatge indicar el SOP Class UID que toqui
        // ara mateix no es una informacio rellevant ja que no la llegim.
        newKeyImageNoteDocument->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("1.2.840.10008.5.1.4.1.1.2", qPrintable(image->getSOPInstanceUID())));
        newKeyImageNoteDocument->getCurrentRequestedProcedureEvidence().addItem(newKeyImageNoteDocument->getStudyInstanceUID(), newKeyImageNoteDocument->getSeriesInstanceUID(), "1.2.840.10008.5.1.4.1.1.2", qPrintable(image->getSOPInstanceUID()));
    }
}

void KeyImageNoteManager::fillStructuredReportCommonData(DSRDocument *newKeyImageNoteDocument, Series *newKeyImageNoteSeries)
{
    Study *parentStudy = m_currentSelection[0]->getParentSeries()->getParentStudy();
    Patient *parentPatient = parentStudy->getParentPatient();
    KeyImageNote *newKeyImageNote = newKeyImageNoteSeries->getKeyImageNotes().at(0);
    
    newKeyImageNoteDocument->createNewSeriesInStudy(qPrintable(parentStudy->getInstanceUID()));

    // Emplenem dades del modul de Patient
    newKeyImageNoteDocument->setPatientID(qPrintable(parentPatient->getID()));
    newKeyImageNoteDocument->setPatientsName(qPrintable(parentPatient->getFullName()));
    newKeyImageNoteDocument->setPatientsBirthDate(qPrintable(parentPatient->getBirthDate().toString("yyyyMMdd")));
    newKeyImageNoteDocument->setPatientsSex(qPrintable(parentPatient->getSex()));
    
    // Emplenem dades del modul de Study
    newKeyImageNoteDocument->setAccessionNumber(qPrintable(parentStudy->getAccessionNumber()));
    newKeyImageNoteDocument->setStudyDescription(qPrintable(parentStudy->getDescription()));
    newKeyImageNoteDocument->setStudyID(qPrintable(parentStudy->getID()));

    // Emplenem els UID ja que ara ja els hem generat
    newKeyImageNote->setInstanceUID(QString(newKeyImageNoteDocument->getSOPInstanceUID()));
    newKeyImageNoteSeries->setInstanceUID(QString(newKeyImageNoteDocument->getSeriesInstanceUID()));
    newKeyImageNoteSeries->setSeriesNumber(QString("0000")+ QString::number(parentStudy->getSeries().count()));

    foreach (DICOMReferencedImage *referencedImage, newKeyImageNote->getDICOMReferencedImages())
    {
        referencedImage->setReferenceParentSOPInstanceUID(newKeyImageNote->getInstanceUID());
    }
    
    parentStudy->addSeries(newKeyImageNoteSeries);
}

int KeyImageNoteManager::getNumberOfImagesInCurrentSelection()
{
    return m_currentSelection.count();
}
}