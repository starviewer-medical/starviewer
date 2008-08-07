#include "dicommask.h"

#include <QStringList>
#include <QDateTime>
#include <QDate>
#include <dcsequen.h>
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#include <QString>

#include "errordcmtk.h"
#include "status.h"
#include "logging.h"

namespace udg{

DicomMask::DicomMask()
{
}

/************************* PATIENT  *********************************************************************/

void DicomMask::setPatientId(QString patientId)
{
    m_patientId = patientId;
}

void DicomMask::setPatientName(QString patientName)
{
    m_patientName = patientName;
}

void DicomMask::setPatientBirth(QString patientBirth)
{
    m_patientBirth = patientBirth;
}

void DicomMask::setPatientSex(QString patientSex)
{
    m_patientSex = patientSex;
}

void DicomMask::setPatientAge(QString patientAge)
{
    m_patientAge = patientAge;
}


QString DicomMask::getPatientId() const
{
    return m_patientId;
}

QString DicomMask::getPatientName() const
{
    return m_patientName;
}

QString DicomMask::getPatientBirth() const
{
    return m_patientBirth;
}

QString DicomMask::getPatientSex() const
{
    return m_patientSex;
}

QString DicomMask::getPatientAge() const
{
    return m_patientAge;
}

/****************************************** STUDY *****************************************************/

void DicomMask:: setStudyId(QString studyId)
{
    m_studyId = studyId;
}

void DicomMask:: setStudyDate(QString studyDate)
{
    m_studyDate = studyDate;
}

void DicomMask::setStudyDescription(QString studyDescription)
{
    m_studyDescription = studyDescription;
}

void DicomMask::setStudyModality(QString studyModality)
{
    m_studyModality = studyModality;
}

void DicomMask:: setStudyTime(QString studyTime)
{
    m_studyTime = studyTime;
}

void DicomMask:: setStudyUID(QString studyUID)
{
    m_studyUID = studyUID;
}

void DicomMask:: setAccessionNumber(QString accessionNumber)
{
    m_accessionNumber = accessionNumber;
}

void DicomMask::setReferringPhysiciansName(QString referringPhysiciansName)
{
    m_referringPhysiciansName = referringPhysiciansName;
}

/***************************************************************   GET **********************************************/

QString DicomMask::getStudyUID() const
{
    return m_studyUID;
}

QString DicomMask::getStudyId() const
{
    return m_studyId;
}

QString DicomMask::getStudyDate() const
{
    return m_studyDate;
}

QString DicomMask::getStudyDescription() const
{
    return m_studyDescription;
}

QString DicomMask::getStudyTime() const
{
    return m_studyTime;
}

QString DicomMask::getStudyModality() const
{
    return m_studyModality;
}

QString DicomMask::getAccessionNumber() const
{
    return m_accessionNumber;
}

QString DicomMask::getReferringPhysiciansName() const
{
    return m_referringPhysiciansName;
}

/************************************** SERIES *************************************************/

void DicomMask:: setSeriesNumber(QString seriesNumber)
{
    m_seriesNumber = seriesNumber;
}

void DicomMask:: setSeriesDate(QString seriesDate)
{
    m_seriesDate = seriesDate;
}

void DicomMask:: setSeriesDescription(QString seriesDescription)
{
    m_seriesDescription = seriesDescription;
}

void DicomMask:: setSeriesModality(QString seriesModality)
{
    m_seriesModality = seriesModality;
}

void DicomMask:: setSeriesTime(QString seriesTime)
{
    m_seriesTime = seriesTime;
}

void DicomMask:: setSeriesUID(QString seriesUID)
{
    m_seriesUID = seriesUID;
}

void DicomMask:: setSeriesProtocolName(QString seriesProtocolName)
{
    m_seriesProtocolName = seriesProtocolName;
}

void DicomMask::setRequestAttributeSequence(QString requestedProcedureID, QString scheduledProcedureStepID)
{
    m_requestedProcedureID = requestedProcedureID;
    m_scheduledProcedureStepID = scheduledProcedureStepID;
}

void DicomMask::setPPSStartDate(QString PPPSStartDate)
{
    m_PPSStartDate = PPPSStartDate;
}

void DicomMask::setPPStartTime(QString PPSStartTime)
{
    m_PPSStartTime = PPSStartTime;
}

/************************************************ GET **************************************************************/

QString DicomMask::getSeriesNumber() const
{
    return m_seriesNumber;
}

QString DicomMask::getSeriesDate() const
{
    return m_seriesDate;
}

QString DicomMask::getSeriesTime() const
{
    return m_seriesTime;
}

QString DicomMask::getSeriesDescription() const
{
    return m_seriesDescription;
}

QString DicomMask::getSeriesModality() const
{
    return m_seriesModality;
}


QString DicomMask::getSeriesProtocolName() const
{
    return m_seriesProtocolName;
}

QString DicomMask::getSeriesUID() const
{
    return m_seriesUID;
}

QString DicomMask::getRequestedProcedureID() const
{
    return m_requestedProcedureID;
}

QString DicomMask::getScheduledProcedureStepID() const
{
    return m_scheduledProcedureStepID;
}

QString DicomMask::getPPSStartDate() const
{
    return m_PPSStartDate;
}

QString DicomMask::getPPSStartTime() const
{
    return m_PPSStartTime;
}

/********************************************** IMAGE **************************************/

void DicomMask:: setImageNumber(QString imageNumber)
{
    m_imageNumber = imageNumber;
}

void DicomMask:: setSOPInstanceUID(QString SOPInstanceUID)
{
    m_SOPInstanceUID = SOPInstanceUID;
}

QString DicomMask::getImageNumber() const
{
    return m_imageNumber;
}

QString DicomMask::getSOPInstanceUID() const
{
    return m_SOPInstanceUID;
}

DcmDataset* DicomMask::getDicomMask()
{
    DcmDataset *maskDcmDataset = new DcmDataset();

    if (!getPatientId().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PatientID);
        elem->putString(qPrintable(getPatientId()));
        maskDcmDataset->insert(elem, OFTrue);
    }

    if (!getPatientName().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PatientsName);
        elem->putString(qPrintable(getPatientName()));
        maskDcmDataset->insert(elem, OFTrue);
    }

    if (!getPatientBirth().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PatientsBirthDate);
        elem->putString(qPrintable(getPatientBirth()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getPatientSex().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PatientsSex);
        elem->putString(qPrintable(getPatientSex()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getPatientAge().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PatientsAge);
        elem->putString(qPrintable(getPatientAge()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getStudyId().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_StudyID);
        elem->putString(qPrintable(getStudyId()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getStudyDescription().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_StudyDescription);
        elem->putString(qPrintable(getStudyDescription()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getStudyModality().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_ModalitiesInStudy);
        elem->putString(qPrintable(getStudyModality()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getStudyDate().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_StudyDate);
        elem->putString(qPrintable(getStudyDate()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getStudyTime().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_StudyTime);
        elem->putString(qPrintable(getStudyTime()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getAccessionNumber().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_AccessionNumber);
        elem->putString(qPrintable(getAccessionNumber()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getReferringPhysiciansName().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_ReferringPhysiciansName);
        elem->putString(qPrintable(getReferringPhysiciansName()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getStudyUID().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_StudyInstanceUID);
        elem->putString(qPrintable(getStudyUID()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesNumber().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_SeriesNumber);
        elem->putString(qPrintable(getSeriesNumber()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesDate().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_SeriesDate);
        elem->putString(qPrintable(getSeriesDate()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesModality().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_Modality);
        elem->putString(qPrintable(getSeriesModality()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesTime().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_SeriesTime);
        elem->putString(qPrintable(getSeriesTime()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesDescription().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_SeriesDescription);
        elem->putString(qPrintable(getSeriesDescription()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesProtocolName().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_ProtocolName);
        elem->putString(qPrintable(getSeriesProtocolName()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSeriesUID().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_SeriesInstanceUID);
        elem->putString(qPrintable(getSeriesUID()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getRequestedProcedureID().isNull() || !getScheduledProcedureStepID().isNull())
    {
        DcmSequenceOfItems *requestedAttributeSequence = new DcmSequenceOfItems(DCM_RequestAttributesSequence);

        DcmItem *requestedAttributeSequenceItem = new DcmItem(DCM_Item);
        requestedAttributeSequenceItem->putAndInsertString(DCM_RequestedProcedureID, qPrintable(getRequestedProcedureID()));

        requestedAttributeSequenceItem->putAndInsertString(DCM_ScheduledProcedureStepID, qPrintable(getScheduledProcedureStepID()));

        requestedAttributeSequence->insert(requestedAttributeSequenceItem);
        maskDcmDataset->insert(requestedAttributeSequence, OFTrue);
    }

    if (!getPPSStartDate().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PerformedProcedureStepStartDate);
        elem->putString(qPrintable(getPPSStartDate()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getPPSStartTime().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_PerformedProcedureStepStartTime);
        elem->putString(qPrintable(getPPSStartTime()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getSOPInstanceUID().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_SOPInstanceUID);
        elem->putString(qPrintable(getSOPInstanceUID()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    if (!getImageNumber().isNull())
    {
        DcmElement *elem = newDicomElement(DCM_InstanceNumber);
        elem->putString(qPrintable(getImageNumber()));
        maskDcmDataset->insert(elem , OFTrue);
    }

    /*Especifiquem a quin nivell es fa el QueryRetrieve, a través del mètode getQueryRetrieveLevel, que ens retorna el nivell en funció dels camps de la màscara*/
    DcmElement *elem = newDicomElement(DCM_QueryRetrieveLevel);
    elem->putString(qPrintable(getQueryRetrieveLevel()));
    maskDcmDataset->insert(elem , OFTrue);

    return maskDcmDataset;
}

bool DicomMask::operator ==(const DicomMask &mask)
{
    if(   getStudyUID() == mask.getStudyUID()
        && getPatientId() == mask.getPatientId()
        && getPatientName() == mask.getPatientName()
        && getPatientBirth() == mask.getPatientBirth()
        && getPatientAge() == mask.getPatientAge()
        && getPatientSex() == mask.getPatientSex()
        && getStudyId() == mask.getStudyId()
        && getStudyDate() == mask.getStudyDate()
        && getStudyDescription() == mask.getStudyDescription()
        && getStudyModality() == mask.getStudyModality()
        && getStudyTime() == mask.getStudyTime()
        && getAccessionNumber() == mask.getAccessionNumber()
        && getReferringPhysiciansName() == mask.getReferringPhysiciansName()
        && getSeriesNumber() == mask.getSeriesNumber()
        && getSeriesDate() == mask.getSeriesDate()
        && getSeriesTime() == mask.getSeriesTime()
        && getSeriesDescription() == mask.getSeriesDescription()
        && getSeriesModality() == mask.getSeriesModality()
        && getSeriesProtocolName() == mask.getSeriesProtocolName()
        && getSeriesUID() == mask.getSeriesUID()
        && getRequestedProcedureID() == mask.getRequestedProcedureID()
        && getScheduledProcedureStepID() == mask.getScheduledProcedureStepID()
        && getPPSStartDate() == mask.getPPSStartDate()
        && getPPSStartTime() == mask.getPPSStartTime()
        && getImageNumber() == mask.getImageNumber()
        && getSOPInstanceUID() == mask.getSOPInstanceUID()
   )
        return true;
    else
        return false;
}

bool DicomMask::isAHeavyQuery()
{
    //en aquest QStringList hi posarem els noms. cognoms i strings que són potencialment candidats a donar cerques pesades
    QStringList heavyWords;
    
    //\TODO per fer-ho correctament aquest "diccionari" de paraules candidates a fer cerques pesades hauria d'estar en un fitxer, de manera que l'usuari no hagi de manipular codi per afegir un nou terme.
//     heavyWords << "joan" << "juan" << "josep" << "jose" << "ana" << "antoni" << "antonio" << "garcia" << "ez";
    
    bool longPeriod = false;
    
    QString studyDate = getStudyDate();
    QString patientName = getPatientName();
    
    /// Condicions que determinen si una query és pesada:
    /// SENSE DATA D'ESTUDI ESPECIFICADA (ANY DATE)
    bool anyDate = studyDate.length() == 0;
    
    /// SENSE ESPECIFICAR NOM
    bool noName = patientName == "*";
        
    /// SENSE ID DE PACIENT
    bool noID = getPatientId() == "*";
    
    /// PERÍODE RELATIVAMENT LLARG 
    if (studyDate.length() > 8)
    {
        QDateTime begin(QDate(studyDate.mid(0, 4).toInt(), studyDate.mid(4, 2).toInt(), studyDate.mid(6, 2).toInt()));
        QDateTime end(QDate(studyDate.mid(10, 4).toInt(), studyDate.mid(14, 2).toInt(), studyDate.mid(16, 2).toInt()));
        
        //consederem com a període llarg a partir d'una setmana
        longPeriod = end.daysTo(begin) > 7;
    }

    ///NOM CURT
    QString nameWithoutAst =  patientName.remove(QChar('*'));
    bool shortName = (nameWithoutAst.length() < 4);
    
    //EL NOM ÉS UN STRING DELS DETERMINATS COM A PESATS A L'INICI DEL MÈTODE
    //no ho tenim en compte perquè en un hospital seran uns noms i en un altre hospital seran uns altres
    //bool heavyName = singleWordAsName && heavyWords.contains (nameWithoutAst, Qt::CaseInsensitive); 
    
    ///EL NÚMERO D'ESTUDI ÉS CURT
    QString idWithoutAst =  getPatientId().remove(QChar('*'));
    bool shortID = idWithoutAst.length() < 3;
    
    //Construïm les condicions que fan que una query pugui ser pesada
    bool noIDOrName = (noName && noID) || (shortName && shortID);
    
    bool heavyMask =  (anyDate  && noIDOrName) || (longPeriod && noIDOrName);
    
    return heavyMask;
}

QString DicomMask::getQueryRetrieveLevel()
{
    bool isImageLevel = !getSOPInstanceUID().isNull() || !getImageNumber().isNull();
    bool isSeriesLevel = !getSeriesDescription().isNull()  || !getSeriesDate().isNull() || !getSeriesModality().isNull() ||
                         !getSeriesNumber().isNull() || !getSeriesProtocolName().isNull() || !getSeriesTime().isNull() ||
                         !getSeriesUID().isNull() || !getRequestedProcedureID().isNull() || !getScheduledProcedureStepID().isNull() ||
                         !getPPSStartDate().isNull() || !getPPSStartTime().isNull();

    if (isImageLevel)
    {
        return "IMAGE";
    }
    else if (isSeriesLevel)
    {
        return "SERIES";
    }
    else return "STUDY"; //PER DEFECTE DEL DICOM COM A MÍNIM SON A NIVELL D'ESTUDI
}

};

