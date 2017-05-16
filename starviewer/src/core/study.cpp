/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "study.h"
#include "patient.h"
#include "logging.h"

#include <QStringList>

namespace udg {

Study::Study(Patient *parentPatient, QObject *parent)
 : QObject(parent), m_height(0), m_weight(0), m_parentPatient(parentPatient)
{
}

Study::~Study()
{
}

void Study::setParentPatient(Patient *patient)
{
    m_parentPatient = patient;
    this->setParent(m_parentPatient);
}

Patient* Study::getParentPatient() const
{
    return m_parentPatient;
}

void Study::setInstanceUID(QString uid)
{
    m_studyInstanceUID = uid;
}

QString Study::getInstanceUID() const
{
    return m_studyInstanceUID;
}

void Study::setID(QString id)
{
    m_studyID = id;
}

QString Study::getID() const
{
    return m_studyID;
}

void Study::setAccessionNumber(QString accessionNumber)
{
    m_accessionNumber = accessionNumber;
}

QString Study::getAccessionNumber() const
{
    return m_accessionNumber;
}

void Study::setDescription(QString description)
{
    m_description = description;
}

QString Study::getDescription() const
{
    return m_description;
}

void Study::setPatientAge(const QString &age)
{
    m_age = age;
}

QString Study::getPatientAge() const
{
    QString age = m_age;
    
    if (age.trimmed().isEmpty())
    {
        if (getParentPatient())
        {
            QDate birthDate = getParentPatient()->getBirthDate();

            if (birthDate.isValid() && m_date.isValid())
            {
                int ageInDays = birthDate.daysTo(m_date);

                if (ageInDays > 0)
                {
                    QDate dateInDays(1, 1, 1);
                    dateInDays = dateInDays.addDays(ageInDays);

                    // If age is > 24 months, age is displayed in years as "xY" where x is the age in years
                    // If age is between 3 to 24 months it is displayed as "xM" where x is the age in months
                    // If age is between 1 to 3 months it is displayed as "xW" where x is the age in weeks
                    // If it is less that 1 month it is displayed as "xD" where x is the age in days. 
                    int quantity = 0;
                    QString unit;
                    if (dateInDays.year() - 1 < 2)
                    {
                        if (ageInDays < 31)
                        {
                            quantity = ageInDays;
                            unit = "D";
                        }
                        else
                        {
                            int months = dateInDays.month() - 1;
                            if (dateInDays.year() - 1 == 1)
                            {
                                months += 12;
                            }
                        
                            if (months < 3)
                            {
                                quantity = dateInDays.weekNumber();
                                unit = "W";
                            }
                            else
                            {
                                quantity = months;
                                unit = "M";
                            }
                        }
                    }
                    else
                    {
                        quantity = dateInDays.year() - 1;
                        unit = "Y";
                    }

                    age = QString("%1").arg(quantity, 3, 10, QChar('0')) + unit;
                }
            }
        }
    }
    
    return age;
}

void Study::setWeight(double weight)
{
    m_weight = weight;
}

double Study::getWeight() const
{
    return m_weight;
}

void Study::setHeight(double height)
{
    m_height = height;
}

double Study::getHeight() const
{
    return m_height;
}

void Study::addModality(const QString &modality)
{
    if (!m_modalities.contains(modality) && !modality.isEmpty())
    {
        m_modalities << modality;
    }
}

QString Study::getModalitiesAsSingleString() const
{
    return m_modalities.join("/");
}

QStringList Study::getModalities() const
{
    return m_modalities;
}

void Study::setReferringPhysiciansName(QString referringPhysiciansName)
{
    m_referringPhysiciansName = referringPhysiciansName;
}

QString Study::getReferringPhysiciansName() const
{
    return m_referringPhysiciansName;
}

bool Study::setDateTime(int day, int month, int year, int hour, int minute, int second)
{
    return this->setDate(day, month, year) && this->setTime(hour, minute, second);
}

bool Study::setDateTime(QString date, QString time)
{
    return this->setDate(date) && this->setTime(time);
}

bool Study::setDate(int day, int month, int year)
{
    return this->setDate(QDate(year, month, day));
}

bool Study::setDate(QString date)
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format yyyy.MM.dd
    return this->setDate(QDate::fromString(date.remove("."), "yyyyMMdd"));
}

bool Study::setDate(QDate date)
{
    if (date.isValid())
    {
        m_date = date;
        return true;
    }
    else
    {
        DEBUG_LOG("La data està en un mal format");
        return false;
    }
}

bool Study::setTime(int hour, int minute, int second)
{
    return this->setTime(QTime(hour, minute, second));
}

bool Study::setTime(QString time)
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format hh:mm:ss.frac
    time = time.remove(":");

    QStringList split = time.split(".");
    QTime convertedTime = QTime::fromString(split[0], "hhmmss");

    // Té fracció al final
    if (split.size() == 2)
    {
        // Trunquem a milisegons i no a milionèssimes de segons
        convertedTime = convertedTime.addMSecs(split[1].leftJustified(3, '0', true).toInt());
    }

    return this->setTime(convertedTime);
}

bool Study::setTime(QTime time)
{
    if (time.isValid())
    {
        m_time = time;
        return true;
    }
    else
    {
        DEBUG_LOG("El time està en un mal format");
        return false;
    }
}

QDate Study::getDate() const
{
    return m_date;
}

QString Study::getDateAsString()
{
    return m_date.toString(Qt::LocaleDate);
}

QTime Study::getTime() const
{
    return m_time;
}

QString Study::getTimeAsString()
{
    return m_time.toString("HH:mm:ss");
}

QDateTime Study::getDateTime() const
{
    return QDateTime(m_date, m_time);
}

void Study::setRetrievedDate(QDate retrievedDate)
{
    m_retrievedDate = retrievedDate;
}

void Study::setRetrievedTime(QTime retrievedTime)
{
    m_retrieveTime = retrievedTime;
}

QDate Study::getRetrievedDate() const
{
    return m_retrievedDate;
}

QTime Study::getRetrievedTime() const
{
    return m_retrieveTime;
}

void Study::setInstitutionName(const QString &institutionName)
{
    m_institutionName = institutionName;
}

QString Study::getInstitutionName() const
{
    return m_institutionName;
}

bool Study::addSeries(Series *series)
{
    bool ok = true;
    QString uid = series->getInstanceUID();
    if (uid.isEmpty())
    {
        ok = false;
        DEBUG_LOG("L'uid de la sèrie està buida! No la podem insertar per inconsistent");
    }
    else if (this->seriesExists(uid))
    {
        ok = false;
        DEBUG_LOG("Ja existeix una sèrie amb aquest mateix UID:: " + uid);
    }
    else
    {
        series->setParentStudy(this);
        this->insertSeries(series);
    }

    return ok;
}

void Study::removeSeries(QString uid)
{
    int index = this->findSeriesIndex(uid);
    if (index != -1)
    {
        m_seriesSet.removeAt(index);
    }
}

Series* Study::getSeries(QString uid)
{
    int index = this->findSeriesIndex(uid);
    if (index != -1)
    {
        return m_seriesSet[index];
    }
    else
    {
        return NULL;
    }
}

bool Study::seriesExists(QString uid)
{
    if (this->findSeriesIndex(uid) != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<Series*> Study::getSelectedSeries()
{
    QList<Series*> seriesList;
    foreach (Series *series, m_seriesSet)
    {
        if (series->isSelected())
        {
            seriesList.append(series);
        }
    }
    return seriesList;
}

int Study::getNumberOfSeries()
{
    return m_seriesSet.size();
}

QList<Series*> Study::getSeries() const
{
    return m_seriesSet;
}

QList<Series*> Study::getViewableSeries()
{
    QList<Series*> result;
    foreach (Series *series, m_seriesSet)
    {
        if (series->isViewable())
        {
            result << series;
        }
    }
    return result;
}

QList<Volume*> Study::getVolumesList()
{
    QList<Volume*> volumesList;
    foreach (Series* serie, m_seriesSet)
    {
        volumesList << serie->getVolumesList();
    }
    return volumesList;
}

int Study::getNumberOfVolumes()
{
    int sum = 0;
    foreach (Series* serie, m_seriesSet)
    {
        sum += serie->getNumberOfVolumes();
    }
    return sum;
}

void Study::setDICOMSource(const DICOMSource &studyDICOMSource)
{
    m_studyDICOMSource = studyDICOMSource;
}

DICOMSource Study::getDICOMSource() const
{
    DICOMSource resultDICOMSource;

    foreach (Series *series, m_seriesSet)
    {
        resultDICOMSource.addPACSDeviceFromDICOMSource(series->getDICOMSource());
    }

    resultDICOMSource.addPACSDeviceFromDICOMSource(m_studyDICOMSource);

    return resultDICOMSource;
}

QString Study::toString()
{
    QString result;

    result += "    - Study\n";
    result += "        InstanceUID : " + getInstanceUID() + "\n";
    result += "        Description : " + getDescription() + "\n";

    foreach (Series *series, getSeries())
    {
        result += series->toString();
    }

    return result;
}

bool Study::operator<(const Study &study)
{
    return getDateTime() < study.getDateTime();
}

bool Study::operator>(const Study &study)
{
    return getDateTime() > study.getDateTime();
}

bool studyIsLessThan(Study *study1, Study *study2)
{
    return *study1 < *study2;
}

bool studyIsGreaterThan(Study *study1, Study *study2)
{
    return *study1 > *study2;
}

QList<Study*> Study::sortStudies(const QList<Study*> &studiesList, StudySortType sortCriteria)
{
    QList<Study*> sortedStudies = studiesList;
    
    switch (sortCriteria)
    {
        case RecentStudiesFirst:
            qSort(sortedStudies.begin(), sortedStudies.end(), studyIsGreaterThan);
            break;

        case OlderStudiesFirst:
            qSort(sortedStudies.begin(), sortedStudies.end(), studyIsLessThan);
            break;
    }

    return sortedStudies;
}

void Study::insertSeries(Series *series)
{
    int i = 0;
    while (i < m_seriesSet.size() && m_seriesSet.at(i)->getSeriesNumber().toInt() < series->getSeriesNumber().toInt())
    {
        ++i;
    }
    if (!m_modalities.contains(series->getModality()))
    {
        m_modalities << series->getModality();
    }

    m_seriesSet.insert(i, series);
}

int Study::findSeriesIndex(QString uid)
{
    int i = 0;
    bool found = false;
    while (i < m_seriesSet.size() && !found)
    {
        if (m_seriesSet.at(i)->getInstanceUID() == uid)
        {
            found = true;
        }
        else
        {
            ++i;
        }
    }
    if (!found)
    {
        i = -1;
    }

    return i;
}

}
