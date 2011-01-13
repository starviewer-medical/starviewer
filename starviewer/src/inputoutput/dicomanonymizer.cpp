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
}

DICOMAnonymizer::~DICOMAnonymizer()
{
    delete gdcmAnonymizer;
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
    gdcmAnonymizer = new gdcm::gdcmAnonymizerStarviewer();
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

bool DICOMAnonymizer::anonymyzeDICOMFilesDirectory(QString directoryPath)
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

bool DICOMAnonymizer::anonymizeDICOMFile(QString inputPathFile, QString outputPathFile)
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

    gdcmAnonymizer->SetFile(gdcmFile);
    if (!gdcmAnonymizer->BasicApplicationLevelConfidentialityProfile(true))
    {
        ERROR_LOG("No s'ha pogut anonimitzar el fitxer " + inputPathFile);
        return false;
    }

    gdcmAnonymizer->Replace(gdcm::Tag(0x0010, 0x0010), "Anonymous"); //Establi el mom del pacient anonimitzat
    
    if (getReplacePatientIDInsteadOfRemove())
    {
        gdcmAnonymizer->Replace(gdcm::Tag(0x0010, 0x0020), qPrintable(getAnonimyzedPatientID(originalPatientID))); //ID Pacient
    }

    if (getReplaceStudyIDInsteadOfRemove())
    {
        gdcmAnonymizer->Replace(gdcm::Tag(0x0020, 0x0010), qPrintable(getAnonymizedStudyID(originalStudyInstanceUID))); //ID Estudi
    }

    if (getRemovePrivateTags())
    {
        if (!gdcmAnonymizer->RemovePrivateTags())
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

QString DICOMAnonymizer::getAnonimyzedPatientID(QString originalPatientID)
{
    if (!hashOriginalPatientIDToAnonimyzedPatientID.contains(originalPatientID))
    {
        hashOriginalPatientIDToAnonimyzedPatientID.insert(originalPatientID, QString::number(hashOriginalPatientIDToAnonimyzedPatientID.count() + 1));
    }
    
    return hashOriginalPatientIDToAnonimyzedPatientID.value(originalPatientID);
}

QString DICOMAnonymizer::getAnonymizedStudyID(QString originalStudyInstanceUID)
{
    if (!hashOriginalStudyInstanceUIDToAnonimyzedStudyID.contains(originalStudyInstanceUID))
    {
        hashOriginalStudyInstanceUIDToAnonimyzedStudyID.insert(originalStudyInstanceUID, QString::number(hashOriginalStudyInstanceUIDToAnonimyzedStudyID.count() + 1));
    }

    return hashOriginalStudyInstanceUIDToAnonimyzedStudyID.value(originalStudyInstanceUID);
}


QString DICOMAnonymizer::readTagValue(gdcm::File *gdcmFile, gdcm::Tag tagToRead)
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