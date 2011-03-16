#include "dicomanonymizer.h"

#include <gdcmGlobal.h>
#include <gdcmUIDGenerator.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#include <gdcmDefs.h>
#include <QCoreApplication>
#include <QDir>
#include <dcuid.h>

#include "logging.h"

namespace udg {

DICOMAnonymizer::DICOMAnonymizer()
{
    initializeGDCM();

    m_replacePatientIDInsteadOfRemove = false;
    m_replaceStudyIDInsteadOfRemove = false;
    m_removePritaveTags = true;
    m_patientNameAnonymized = "";
}

DICOMAnonymizer::~DICOMAnonymizer()
{
    delete m_gdcmAnonymizer;
}

void DICOMAnonymizer::setPatientNameAnonymized(const QString &patientNameAnonymized)
{
    m_patientNameAnonymized = patientNameAnonymized;
}

QString DICOMAnonymizer::getPatientNameAnonymized() const
{
    return m_patientNameAnonymized;
}

void DICOMAnonymizer::setReplacePatientIDInsteadOfRemove(bool replace)
{
    m_replacePatientIDInsteadOfRemove = replace;
}

bool DICOMAnonymizer::getReplacePatientIDInsteadOfRemove()
{
    return m_replacePatientIDInsteadOfRemove;
}

void DICOMAnonymizer::setReplaceStudyIDInsteadOfRemove(bool replace)
{
    m_replaceStudyIDInsteadOfRemove = replace;
}

bool DICOMAnonymizer::getReplaceStudyIDInsteadOfRemove()
{
    return m_replaceStudyIDInsteadOfRemove;
}

void DICOMAnonymizer::setRemovePrivateTags(bool removePritaveTags)
{
    m_removePritaveTags = removePritaveTags;
}

bool DICOMAnonymizer::getRemovePrivateTags()
{
    return m_removePritaveTags;
}

void DICOMAnonymizer::initializeGDCM()
{
    m_gdcmAnonymizer = new gdcm::gdcmAnonymizerStarviewer();
    gdcm::Global *gdcmGlobalInstance = &gdcm::Global::GetInstance();

    //Indiquem el directori on pot trobar el fitxer part3.xml que és un diccionari DICOM.
    //TODO: On posem el fitxer part3.xml
    gdcmGlobalInstance->Prepend(qPrintable(QCoreApplication::applicationDirPath()));

    //Carrega el fitxer part3.xml
    if (!gdcmGlobalInstance->LoadResourcesFiles())
    {
        ERROR_LOG("No s'ha trobat el fitxer part3.xml a " + QCoreApplication::applicationDirPath());
    }

    const gdcm::Defs &defs = gdcmGlobalInstance->GetDefs(); 
    (void)defs;
    //TODO:utilitzem el UID de dcmtk hauríem de tenir el nostre propi això també passa a VolumeBuilderFromCaptures
    if (!gdcm::UIDGenerator::IsValid(SITE_UID_ROOT))
    {
        ERROR_LOG(QString("No es pot anonimitzar els fitxers DICOM perquè el UID arrel per crear els nous fitxers no es valid %1").arg(SITE_UID_ROOT));
    }

    gdcm::UIDGenerator::SetRoot(SITE_UID_ROOT);
}

bool DICOMAnonymizer::anonymyzeDICOMFilesDirectory(const QString &directoryPath)
{
    QDir directory;
    directory.setPath(directoryPath);

    foreach(QFileInfo entryInfo, directory.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files))
    {
        if (entryInfo.isDir())
        {
            if (!anonymyzeDICOMFilesDirectory(entryInfo.absoluteFilePath()))
            {
                return false;
            }
        }
        else
        {
            if (!anonymizeDICOMFile(entryInfo.absoluteFilePath(), entryInfo.absoluteFilePath()))
            {
                return false;
            }
        }
    }

    return true;
}

bool DICOMAnonymizer::anonymizeDICOMFile(const QString &inputPathFile, const QString &outputPathFile)
{
    gdcm::Reader gdcmReader;
    gdcmReader.SetFileName(qPrintable(inputPathFile));

    if (!gdcmReader.Read())
    {
        ERROR_LOG("No s'ha trobat el fitxer a anonimitzar " + inputPathFile);
        return false;
    }

    gdcm::File &gdcmFile = gdcmReader.GetFile();
    gdcm::MediaStorage gdcmMediaStorage;
    gdcmMediaStorage.SetFromFile(gdcmFile);
    if (!gdcm::Defs::GetIODNameFromMediaStorage(gdcmMediaStorage))
    {
        ERROR_LOG(QString("Media storage type del fitxer no suportat: %1").arg(gdcmMediaStorage.GetString()));
        return false;
    }

    QString originalPatientID = readTagValue(&gdcmFile, gdcm::Tag(0x0010, 0x0020));
    QString originalStudyInstanceUID = readTagValue(&gdcmFile, gdcm::Tag(0x0020, 0x000d));

    m_gdcmAnonymizer->SetFile(gdcmFile);
    if (!m_gdcmAnonymizer->BasicApplicationLevelConfidentialityProfile(true))
    {
        ERROR_LOG("No s'ha pogut anonimitzar el fitxer " + inputPathFile);
        return false;
    }

    m_gdcmAnonymizer->Replace(gdcm::Tag(0x0010, 0x0010), qPrintable(m_patientNameAnonymized)); //Estableix el mom del pacient anonimitzat
    
    if (getReplacePatientIDInsteadOfRemove())
    {
        m_gdcmAnonymizer->Replace(gdcm::Tag(0x0010, 0x0020), qPrintable(getAnonimyzedPatientID(originalPatientID))); //ID Pacient
    }

    if (getReplaceStudyIDInsteadOfRemove())
    {
        m_gdcmAnonymizer->Replace(gdcm::Tag(0x0020, 0x0010), qPrintable(getAnonymizedStudyID(originalStudyInstanceUID))); //ID Estudi
    }

    if (getRemovePrivateTags())
    {
        if (!m_gdcmAnonymizer->RemovePrivateTags())
        {
            ERROR_LOG("No s'ha pogut treure els tags privats del fitxer " + inputPathFile);
            return false;            
        }
    }

    //Regenerem la capçalera DICOM amb el nou SOP Instance UID
    gdcm::FileMetaInformation gdcmFileMetaInformation = gdcmFile.GetHeader();
    gdcmFileMetaInformation.Clear();

    gdcm::Writer gdcmWriter;
    gdcmWriter.SetFileName(qPrintable(outputPathFile));
    gdcmWriter.SetFile(gdcmFile);
    if (!gdcmWriter.Write())
    {
        ERROR_LOG("No s'ha pogut generar el fitxer anonimitzat de " + inputPathFile + " a " + outputPathFile);
        return false;
    }

    return true;
}

QString DICOMAnonymizer::getAnonimyzedPatientID(const QString &originalPatientID)
{
    if (!m_hashOriginalPatientIDToAnonimyzedPatientID.contains(originalPatientID))
    {
        m_hashOriginalPatientIDToAnonimyzedPatientID.insert(originalPatientID, QString::number(m_hashOriginalPatientIDToAnonimyzedPatientID.count() + 1));
    }
    
    return m_hashOriginalPatientIDToAnonimyzedPatientID.value(originalPatientID);
}

QString DICOMAnonymizer::getAnonymizedStudyID(const QString &originalStudyInstanceUID)
{
    if (!m_hashOriginalStudyInstanceUIDToAnonimyzedStudyID.contains(originalStudyInstanceUID))
    {
        m_hashOriginalStudyInstanceUIDToAnonimyzedStudyID.insert(originalStudyInstanceUID, QString::number(m_hashOriginalStudyInstanceUIDToAnonimyzedStudyID.count() + 1));
    }

    return m_hashOriginalStudyInstanceUIDToAnonimyzedStudyID.value(originalStudyInstanceUID);
}


QString DICOMAnonymizer::readTagValue(gdcm::File *gdcmFile, gdcm::Tag tagToRead) const
{
    gdcm::DataElement dataElement = gdcmFile->GetDataSet().GetDataElement(tagToRead);
    QString tagValue = "";

    if(!dataElement.IsEmpty())
    {
        if(gdcm::ByteValue *byteValueTag = dataElement.GetByteValue())
        {
            tagValue = (std::string(byteValueTag->GetPointer(), byteValueTag->GetLength())).c_str();
        }
    }

    return tagValue;
}

}