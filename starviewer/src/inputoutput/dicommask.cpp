#include "dicommask.h"

#include <QStringList>
#include <QDateTime>
#include <QDate>
#include <QString>

namespace udg {

const QString DicomMask::DateFormatAsString("yyyyMMdd");
const QString DicomMask::TimeFormatAsString("HHmmss");

DicomMask::DicomMask()
{
}

/************************* PATIENT  *********************************************************************/

void DicomMask::setPatientId(const QString &patientId)
{
    m_patientId = patientId;
}

void DicomMask::setPatientName(const QString &patientName)
{
    m_patientName = patientName;
}

void DicomMask::setPatientBirth(const QDate &minimumDate, const QDate &maximumDate)
{
    m_patientBirthMinimum = "";
    m_patientBirthMaximum = "";

    if (minimumDate.isValid())
    {
        m_patientBirthMinimum = minimumDate.toString(DateFormatAsString);
    }

    if (maximumDate.isValid())
    {
        m_patientBirthMaximum = maximumDate.toString(DateFormatAsString);
    }
}

void DicomMask::setPatientSex(const QString &patientSex)
{
    m_patientSex = patientSex;
}

void DicomMask::setPatientAge(const QString &patientAge)
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

QDate DicomMask::getPatientBirthMinimum() const
{
    return QDate().fromString(m_patientBirthMinimum, DateFormatAsString);
}

QDate DicomMask::getPatientBirthMaximum() const
{
    return QDate().fromString(m_patientBirthMaximum, DateFormatAsString);
}

QString DicomMask::getPatientBirthRangeAsDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_patientBirthMinimum, m_patientBirthMaximum);
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

void DicomMask::setStudyId(const QString &studyId)
{
    m_studyId = studyId;
}

void DicomMask::setStudyDate(const QDate &minimumDate, const QDate &maximumDate)
{
    m_studyDateMinimum = "";
    m_studyDateMaximum = "";

    if (minimumDate.isValid())
    {
        m_studyDateMinimum = minimumDate.toString(DateFormatAsString);
    }

    if (maximumDate.isValid())
    {
        m_studyDateMaximum = maximumDate.toString(DateFormatAsString);
    }
}

void DicomMask::setStudyDescription(const QString &studyDescription)
{
    m_studyDescription = studyDescription;
}

void DicomMask::setStudyModality(const QString &studyModality)
{
    m_studyModality = studyModality;
}

void DicomMask::setStudyTime(const QTime &studyTimeMinimum, const QTime &studyTimeMaximum)
{
    m_studyTimeMinimum = "";
    m_studyTimeMaximum = "";

    if (studyTimeMinimum.isValid())
    {
        m_studyTimeMinimum = studyTimeMinimum.toString(TimeFormatAsString);
    }
    
    if (studyTimeMaximum.isValid())
    {
        m_studyTimeMaximum = studyTimeMaximum.toString(TimeFormatAsString);
    }
}

void DicomMask::setStudyInstanceUID(const QString &studyInstanceUID)
{
    m_studyInstanceUID = studyInstanceUID;
}

void DicomMask::setAccessionNumber(const QString &accessionNumber)
{
    m_accessionNumber = accessionNumber;
}

void DicomMask::setReferringPhysiciansName(const QString &referringPhysiciansName)
{
    m_referringPhysiciansName = referringPhysiciansName;
}

/***************************************************************   GET **********************************************/

QString DicomMask::getStudyInstanceUID() const
{
    return m_studyInstanceUID;
}

QString DicomMask::getStudyId() const
{
    return m_studyId;
}

QDate DicomMask::getStudyDateMinimum() const
{
    return QDate().fromString(m_studyDateMinimum, DateFormatAsString);
}

QDate DicomMask::getStudyDateMaximum() const
{
    return QDate().fromString(m_studyDateMaximum, DateFormatAsString);
}

QString DicomMask::getStudyDateRangeAsDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_studyDateMinimum, m_studyDateMaximum);
}

QString DicomMask::getStudyDescription() const
{
    return m_studyDescription;
}

QTime DicomMask::getStudyTimeMinimum() const
{
    return QTime().fromString(m_studyTimeMinimum, TimeFormatAsString);
}

QTime DicomMask::getStudyTimeMaximum() const
{
    return QTime().fromString(m_studyTimeMaximum, TimeFormatAsString);
}

QString DicomMask::getStudyTimeRangeAsDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_studyTimeMinimum, m_studyTimeMaximum);
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

void DicomMask::setSeriesNumber(const QString &seriesNumber)
{
    m_seriesNumber = seriesNumber;
}

void DicomMask::setSeriesDate(const QDate &minimumDate, const QDate &maximumDate)
{
    m_seriesDateMinimum = "";
    m_seriesDateMaximum = "";

    if (minimumDate.isValid())
    {
        m_seriesDateMinimum = minimumDate.toString(DateFormatAsString);
    }

    if (maximumDate.isValid())
    {
        m_seriesDateMaximum = maximumDate.toString(DateFormatAsString);
    }
}

void DicomMask::setSeriesDescription(const QString &seriesDescription)
{
    m_seriesDescription = seriesDescription;
}

void DicomMask::setSeriesModality(const QString &seriesModality)
{
    m_seriesModality = seriesModality;
}

void DicomMask::setSeriesTime(const QTime &minimumTime, const QTime &maximumTime)
{
    m_seriesTimeMinimum = "";
    m_seriesTimeMaximum = "";

    if (minimumTime.isValid())
    {
        m_seriesTimeMinimum = minimumTime.toString(TimeFormatAsString);
    }

    if (maximumTime.isValid())
    {
        m_seriesTimeMaximum = maximumTime.toString(TimeFormatAsString);
    }
}

void DicomMask::setSeriesInstanceUID(const QString &seriesInstanceUID)
{
    m_seriesInstanceUID = seriesInstanceUID;
}

void DicomMask::setSeriesProtocolName(const QString &seriesProtocolName)
{
    m_seriesProtocolName = seriesProtocolName;
}

void DicomMask::setRequestAttributeSequence(const QString &requestedProcedureID, const QString &scheduledProcedureStepID)
{
    m_requestedProcedureID = requestedProcedureID;
    m_scheduledProcedureStepID = scheduledProcedureStepID;
}

void DicomMask::setPPSStartDate(const QDate &minimumDate, const QDate &maximumDate)
{
    m_PPSStartDateMinimum = "";
    m_PPSStartDateMaximum = "";

    if (minimumDate.isValid())
    {
        m_PPSStartDateMinimum = minimumDate.toString(DateFormatAsString);
    }

    if (maximumDate.isValid())
    {
        m_PPSStartDateMaximum = maximumDate.toString(DateFormatAsString);
    }
}

void DicomMask::setPPStartTime(const QTime &minimumTime, const QTime &maximumTime)
{
    m_PPSStartTimeMinimum = "";
    m_PPSStartTimeMaximum = "";

    if (minimumTime.isValid())
    {
        m_PPSStartTimeMinimum = minimumTime.toString(TimeFormatAsString);
    }

    if (maximumTime.isValid())
    {
        m_PPSStartTimeMaximum = maximumTime.toString(TimeFormatAsString);
    }
}

/************************************************ GET **************************************************************/

QString DicomMask::getSeriesNumber() const
{
    return m_seriesNumber;
}

QDate DicomMask::getSeriesDateMinimum() const
{
    return QDate().fromString(m_seriesDateMinimum, DateFormatAsString);
}

QDate DicomMask::getSeriesDateMaximum() const
{
    return QDate().fromString(m_seriesDateMaximum, DateFormatAsString);
}

QString DicomMask::getSeriesDateRangeAsDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_seriesDateMinimum, m_seriesDateMaximum);
}

QTime DicomMask::getSeriesTimeMinimum() const
{
    return QTime().fromString(m_seriesTimeMinimum, TimeFormatAsString);
}

QTime DicomMask::getSeriesTimeMaximum() const
{
    return QTime().fromString(m_seriesTimeMaximum, TimeFormatAsString);
}

QString DicomMask::getSeriesTimeRangeAsDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_seriesTimeMinimum, m_seriesTimeMaximum);
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

QString DicomMask::getSeriesInstanceUID() const
{
    return m_seriesInstanceUID;
}

QString DicomMask::getRequestedProcedureID() const
{
    return m_requestedProcedureID;
}

QString DicomMask::getScheduledProcedureStepID() const
{
    return m_scheduledProcedureStepID;
}

QDate DicomMask::getPPSStartDateMinimum() const
{
    return QDate().fromString(m_PPSStartDateMinimum, DateFormatAsString);
}

QDate DicomMask::getPPSStartDateMaximum() const
{
    return QDate().fromString(m_PPSStartDateMaximum, DateFormatAsString);
}

QString DicomMask::getPPSStartDateAsRangeDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_PPSStartDateMinimum, m_PPSStartDateMaximum);
}

QTime DicomMask::getPPSStartTimeMinimum() const
{
    return QTime().fromString(m_PPSStartTimeMinimum, TimeFormatAsString);
}

QTime DicomMask::getPPSStartTimeMaximum() const
{
    return QTime().fromString(m_PPSStartTimeMaximum, TimeFormatAsString);
}

QString DicomMask::getPPSStartTimeAsRangeDICOMFormat() const
{
    return getDateOrTimeRangeAsDICOMFormat(m_PPSStartTimeMinimum, m_PPSStartTimeMaximum);
}

/********************************************** IMAGE **************************************/

void DicomMask::setImageNumber(const QString &imageNumber)
{
    m_imageNumber = imageNumber;
}

void DicomMask::setSOPInstanceUID(const QString &SOPInstanceUID)
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

bool DicomMask::operator ==(const DicomMask &mask)
{
    if (   getStudyInstanceUID() == mask.getStudyInstanceUID()
        && getPatientId() == mask.getPatientId()
        && getPatientName() == mask.getPatientName()
        && getPatientBirthMinimum() == mask.getPatientBirthMinimum()
        && getPatientBirthMaximum() == mask.getPatientBirthMaximum()
        && getPatientAge() == mask.getPatientAge()
        && getPatientSex() == mask.getPatientSex()
        && getStudyId() == mask.getStudyId()
        && getStudyDateMaximum() == mask.getStudyDateMaximum()
        && getStudyDateMinimum() == mask.getStudyDateMinimum()
        && getStudyDescription() == mask.getStudyDescription()
        && getStudyModality() == mask.getStudyModality()
        && getStudyTimeMinimum() == mask.getStudyTimeMinimum()
        && getStudyTimeMaximum() == mask.getStudyTimeMaximum()
        && getAccessionNumber() == mask.getAccessionNumber()
        && getReferringPhysiciansName() == mask.getReferringPhysiciansName()
        && getSeriesNumber() == mask.getSeriesNumber()
        && getSeriesDateMinimum() == mask.getSeriesDateMinimum()
        && getSeriesDateMaximum() == mask.getSeriesDateMaximum()
        && getSeriesTimeMinimum() == mask.getSeriesTimeMinimum()
        && getSeriesTimeMaximum() == mask.getSeriesTimeMaximum()
        && getSeriesDescription() == mask.getSeriesDescription()
        && getSeriesModality() == mask.getSeriesModality()
        && getSeriesProtocolName() == mask.getSeriesProtocolName()
        && getSeriesInstanceUID() == mask.getSeriesInstanceUID()
        && getRequestedProcedureID() == mask.getRequestedProcedureID()
        && getScheduledProcedureStepID() == mask.getScheduledProcedureStepID()
        && getPPSStartDateMinimum() == mask.getPPSStartDateMinimum()
        && getPPSStartDateMaximum() == mask.getPPSStartDateMaximum()
        && getPPSStartTimeMinimum() == mask.getPPSStartTimeMinimum()
        && getPPSStartTimeMaximum() == mask.getPPSStartTimeMaximum()
        && getImageNumber() == mask.getImageNumber()
        && getSOPInstanceUID() == mask.getSOPInstanceUID())
    {
        return true;
    }
    else
    {
        return false;
    }
}

DicomMask DicomMask::operator +(const DicomMask &mask)
{
    DicomMask returnDicomMask = * this;

    // L'operador + no fa que cerquem pels dos valors, sinó que per la màscara this, totes les seves propietats que siguin empties,
    // s'emplenaran amb el valor de la màscara passada per paràmetre

    if (!mask.getPatientAge().isNull() && getPatientAge().isEmpty())
    {
        returnDicomMask.setPatientAge(mask.getPatientAge());
    }
    if (!mask.getPatientBirthRangeAsDICOMFormat().isNull() && getPatientBirthRangeAsDICOMFormat().isEmpty())
    {
        returnDicomMask.setPatientBirth(mask.getPatientBirthMinimum(), mask.getPatientBirthMaximum());
    }
    if (!mask.getPatientId().isNull() && getPatientId().isEmpty())
    {
        returnDicomMask.setPatientId(mask.getPatientId());
    }
    if (!mask.getPatientName().isNull() && getPatientName().isEmpty())
    {
        returnDicomMask.setPatientName(mask.getPatientName());
    }
    if (!mask.getPatientSex().isNull() && getPatientSex().isEmpty())
    {
        returnDicomMask.setPatientSex(mask.getPatientSex());
    }
    if (!mask.getStudyId().isNull() && getStudyId().isEmpty())
    {
        returnDicomMask.setStudyId(mask.getStudyId());
    }
    if (!mask.getStudyDateRangeAsDICOMFormat().isNull() && getStudyDateRangeAsDICOMFormat().isEmpty())
    {
        returnDicomMask.setStudyDate(mask.getStudyDateMinimum(), mask.getStudyDateMaximum());
    }
    if (!mask.getStudyDescription().isNull() && getStudyDescription().isEmpty())
    {
        returnDicomMask.setStudyDescription(mask.getStudyDescription());
    }
    if (!mask.getStudyModality().isNull() && getStudyModality().isEmpty())
    {
        returnDicomMask.setStudyModality(mask.getStudyModality());
    }
    if (!mask.getStudyTimeRangeAsDICOMFormat().isNull() && getStudyTimeRangeAsDICOMFormat().isEmpty())
    {
        returnDicomMask.setStudyTime(mask.getStudyTimeMinimum(), mask.getStudyTimeMaximum());
    }
    if (!mask.getStudyInstanceUID().isNull() && getStudyInstanceUID().isEmpty())
    {
        returnDicomMask.setStudyInstanceUID(mask.getStudyInstanceUID());
    }
    if (!mask.getReferringPhysiciansName().isNull() && getReferringPhysiciansName().isEmpty())
    {
        returnDicomMask.setReferringPhysiciansName(mask.getReferringPhysiciansName());
    }
    if (!mask.getAccessionNumber().isNull() && getAccessionNumber().isEmpty())
    {
        returnDicomMask.setAccessionNumber(mask.getAccessionNumber());
    }
    if (!mask.getSeriesDateRangeAsDICOMFormat().isNull() && getSeriesDateRangeAsDICOMFormat().isEmpty())
    {
        returnDicomMask.setSeriesDate(mask.getSeriesDateMinimum(), mask.getSeriesDateMaximum());
    }
    if (!mask.getSeriesDescription().isNull() && getSeriesDescription().isEmpty())
    {
        returnDicomMask.setSeriesDescription(mask.getSeriesDescription());
    }
    if (!mask.getSeriesModality().isNull() && getSeriesModality().isEmpty())
    {
        returnDicomMask.setSeriesModality(mask.getSeriesModality());
    }
    if (!mask.getSeriesNumber().isNull() && getSeriesNumber().isEmpty())
    {
        returnDicomMask.setSeriesNumber(mask.getSeriesNumber());
    }
    if (!mask.getSeriesProtocolName().isNull() && getSeriesProtocolName().isEmpty())
    {
        returnDicomMask.setSeriesProtocolName(mask.getSeriesProtocolName());
    }
    if (!mask.getSeriesTimeRangeAsDICOMFormat().isNull() && getSeriesTimeRangeAsDICOMFormat().isEmpty())
    {
        returnDicomMask.setSeriesTime(mask.getSeriesTimeMinimum(), mask.getSeriesTimeMaximum());
    }
    if (!mask.getSeriesInstanceUID().isNull() && getSeriesInstanceUID().isEmpty())
    {
        returnDicomMask.setSeriesInstanceUID(mask.getSeriesInstanceUID());
    }
    if (!mask.getRequestedProcedureID().isNull() && getRequestedProcedureID().isEmpty() &&
        !mask.getScheduledProcedureStepID().isNull() && getScheduledProcedureStepID().isEmpty())
    {
        returnDicomMask.setRequestAttributeSequence(mask.getRequestedProcedureID(), mask.getScheduledProcedureStepID());
    }

    if (!mask.getPPSStartDateAsRangeDICOMFormat().isNull() && getPPSStartDateAsRangeDICOMFormat().isEmpty())
    {
        returnDicomMask.setPPSStartDate(mask.getPPSStartDateMinimum(), mask.getPPSStartDateMaximum());
    }
    if (!mask.getPPSStartTimeAsRangeDICOMFormat().isNull() && getPPSStartTimeAsRangeDICOMFormat().isEmpty())
    {
        returnDicomMask.setPPStartTime(mask.getPPSStartTimeMinimum(), mask.getPPSStartTimeMaximum());
    }

    if (!mask.getSOPInstanceUID().isNull() && getSOPInstanceUID().isEmpty())
    {
        returnDicomMask.setSOPInstanceUID(mask.getScheduledProcedureStepID());
    }
    if (!mask.getImageNumber().isNull() && getImageNumber().isEmpty())
    {
        returnDicomMask.setImageNumber(mask.getImageNumber());
    }

    return returnDicomMask;

}

bool DicomMask::isEmpty()
{
    bool empty = m_patientId.isEmpty() && m_patientName.isEmpty() && m_patientBirthMinimum.isEmpty() && m_patientBirthMaximum.isEmpty() && m_patientSex.isEmpty() &&
                 m_patientAge.isEmpty() && m_studyId.isEmpty() && m_studyDateMinimum.isEmpty() && m_studyDateMaximum.isEmpty() && m_studyTimeMinimum.isEmpty() &&
                 m_studyTimeMaximum.isEmpty() && m_studyDescription.isEmpty() && m_studyModality.isEmpty() && m_studyInstanceUID.isEmpty() && 
                 m_accessionNumber.isEmpty() && m_referringPhysiciansName.isEmpty() && m_seriesNumber.isEmpty() && m_seriesDateMinimum.isEmpty() && 
                 m_seriesDateMaximum.isEmpty() && m_seriesTimeMinimum.isEmpty() && m_seriesTimeMaximum.isEmpty() && m_seriesModality.isEmpty() && 
                 m_seriesDescription.isEmpty() && m_seriesProtocolName.isEmpty() && m_seriesInstanceUID.isEmpty()  && m_requestedProcedureID.isEmpty() && 
                 m_scheduledProcedureStepID.isEmpty() && m_PPSStartDateMinimum.isEmpty() && m_PPSStartDateMaximum.isEmpty() && m_PPSStartTimeMinimum.isEmpty() && 
                 m_PPSStartTimeMaximum.isEmpty() && m_SOPInstanceUID.isEmpty() && m_imageNumber.isEmpty();

    return empty;
}

QString DicomMask::getFilledMaskFields() const
{
    QString maskFields;

    if (!QString(m_patientId).remove("*").isEmpty())
    {
        maskFields += "Patient_ID=[#*#] ";
    }
    if (!QString(m_patientName).remove("*").isEmpty())
    {
        maskFields += "Patient_Name=[#*#] ";
    }
    if (!getPatientBirthRangeAsDICOMFormat().isEmpty())
    {
        maskFields += "Patient_Birth=[#" + getPatientBirthRangeAsDICOMFormat() + "#] ";
    }
    if (!QString(m_patientSex).remove("*").isEmpty())
    {
        maskFields += "Patient_Sex=[" + m_patientSex + "] ";
    }
    if (!QString(m_patientAge).remove("*").isEmpty())
    {
        maskFields += "Patient_Age=[" + m_patientAge + "] ";
    }
    if (!QString(m_studyId).remove("*").isEmpty())
    {
        maskFields += "Study_ID=[#*#] ";
    }
    // En el cas de la data fem un tractament especial per fer-ho més llegible i amb més informació
    if (!getStudyDateRangeAsDICOMFormat().isEmpty())
    {
        QDate date;
        QDate today = QDate::currentDate();
        QDate yesterday = QDate::currentDate().addDays(-1);
        QStringList formattedDates;
        maskFields += "Study_Date=[";

        // Si tenim un rang de dates estaran separades per el guió "-"
        // TODO encara no sabem traduir si el rang és només "desde" o "fins" una data
        // per exemple [20090512-] (desde), [-20090611] (fins), i ens ho mostrarà com una data única
        QStringList datesList = getStudyDateRangeAsDICOMFormat().split("-", QString::SkipEmptyParts);
        // "traduim" less dates a un format mé llegible
        foreach (QString dateString, datesList)
        {
            date = QDate::fromString(dateString, DateFormatAsString);
            if (date == today)
            {
                formattedDates << "Today";
            }
            else if (date == yesterday)
            {
                formattedDates << "Yesterday";
            }
            else
            {
                formattedDates << date.toString("dd/MM/yyyy") + " (" + QString::number(date.daysTo(today)) + " days ago)";
            }
        }

        if (formattedDates.count() == 2)
        {
            maskFields += "From " + formattedDates.at(0) + " to " + formattedDates.at(1) + "]";
        }
        else
        {
            maskFields += formattedDates.at(0) + "]";
        }

    }

    if (!getStudyTimeRangeAsDICOMFormat().isEmpty())
    {
        maskFields += "Study_Time=[" + getStudyTimeRangeAsDICOMFormat() + "] ";
    }
    if (!QString(m_studyDescription).remove("*").isEmpty())
    {
        maskFields += "Study_Description=[" + m_studyDescription + "] ";
    }
    if (!QString(m_studyModality).remove("*").isEmpty())
    {
        maskFields += "Study_Modality=[" + m_studyModality + "] ";
    }
    if (!QString(m_studyInstanceUID).remove("*").isEmpty())
    {
        maskFields += "Study_UID=[#*#] ";
    }
    if (!QString(m_accessionNumber).remove("*").isEmpty())
    {
        maskFields += "Accession_Number=[#*#] ";
    }
    if (!QString(m_referringPhysiciansName).remove("*").isEmpty())
    {
        maskFields += "Referring_PhysiciansName=[#*#] ";
    }
    if (!QString(m_seriesNumber).remove("*").isEmpty())
    {
        maskFields += "Series_Number=[#*#] ";
    }
    if (!getSeriesDateRangeAsDICOMFormat().isEmpty())
    {
        maskFields += "Series_Date=[" + getSeriesDateRangeAsDICOMFormat() + "] ";
    }
    if (!getSeriesTimeRangeAsDICOMFormat().isEmpty())
    {
        maskFields += "Series_Time=[" + getSeriesTimeRangeAsDICOMFormat() + "] ";
    }
    if (!QString(m_seriesModality).remove("*").isEmpty())
    {
        maskFields += "Series_Modality=[" + m_seriesModality + "] ";
    }
    if (!QString(m_seriesDescription).remove("*").isEmpty())
    {
        maskFields += "Series_Description=[" + m_seriesDescription + "] ";
    }
    if (!QString(m_seriesProtocolName).remove("*").isEmpty())
    {
        maskFields += "Series_Protocol_Name=[" + m_seriesProtocolName + "] ";
    }
    if (!QString(m_seriesInstanceUID).remove("*").isEmpty())
    {
        maskFields += "Series_UID=[#*#] ";
    }
    if (!QString(m_requestedProcedureID).remove("*").isEmpty())
    {
        maskFields += "Requested_Procedure_ID=[#*#] ";
    }
    if (!QString(m_scheduledProcedureStepID).remove("*").isEmpty())
    {
        maskFields += "Scheduled_Procedure_Step_ID=[#*#] ";
    }
    if (!getPPSStartDateAsRangeDICOMFormat().isEmpty())
    {
        maskFields += "PPS_Start_Date=[#" + getPPSStartDateAsRangeDICOMFormat() + " #] ";
    }
    if (!getPPSStartTimeAsRangeDICOMFormat().isEmpty())
    {
        maskFields += "PPS_Start_Time=[#" + getPPSStartTimeAsRangeDICOMFormat() + "#] ";
    }
    if (!QString(m_SOPInstanceUID).remove("*").isEmpty())
    {
        maskFields += "SOP_Instance_UID=[#*#] ";
    }
    if (!QString(m_imageNumber).remove("*").isEmpty())
    {
        maskFields += "Image_Number=[" + m_imageNumber + "] ";
    }

    return maskFields;
}

QString DicomMask::getDateOrTimeRangeAsDICOMFormat(const QString &minimumDateOrTime, const QString &maximumDateOrTime) const
{
    //El format DICOM per cercar entre dates o hores, el format és Hora/Data mínima + "-" + Hora/Data màxima, per exemple 20110602-20110702
    if (minimumDateOrTime.isNull() && maximumDateOrTime.isNull())
    {
        //No ens han indicat cap filtre, ni que volen que es retorni aquest camp per això retornem un QString Null
        return QString();
    }
    else if (!minimumDateOrTime.isEmpty() && !maximumDateOrTime.isEmpty())
    {
        if (minimumDateOrTime == maximumDateOrTime)
        {
            //Ens han indicat filtrar per un dia o hora concreta, no un rang
            return minimumDateOrTime;
        }
        else
        {
            //hem de filtrar per un rang
            return minimumDateOrTime + "-" + maximumDateOrTime;
        }
    }
    else if (!minimumDateOrTime.isEmpty())
    {
        return minimumDateOrTime + "-";
    }
    else if (!maximumDateOrTime.isEmpty())
    {
        return "-" + maximumDateOrTime;
    }
    else
    {
        //Els dos strings són buits, en aquest cas l'usuari ens han indicat que vol que el PACS retorni aquest camp en els resultats de la consulta, sense filtrar
        return "";
    }
}
};
