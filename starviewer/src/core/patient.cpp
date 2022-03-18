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

#include "patient.h"
#include "logging.h"

#include <QStringList>
#include <QChar>

namespace {

enum EditDistanceMetricGaps { LevenshteinDistance = 1, NeedlemanWunschDistance = 2 };

// Returns the edit cost to make one string equal to the other.
double computeStringEditDistanceMetric(const QString &stringA, const QString &stringB, EditDistanceMetricGaps gap)
{
    // More information about the algorithm:
    // http://staffwww.dcs.shef.ac.uk/people/S.Chapman/stringmetrics.html#Levenshtein
    // http://en.wikipedia.org/wiki/Levenshtein_distance
    // http://staffwww.dcs.shef.ac.uk/people/S.Chapman/stringmetrics.html#needleman
    // http://en.wikipedia.org/wiki/Needleman%E2%80%93Wunsch_algorithm

    int stringALength = stringA.length();
    int stringBLength = stringB.length();

    if (stringALength == 0)
    {
        return 1.;
    }
    else if (stringBLength == 0)
    {
        return 1.;
    }

    int *p = new int[stringALength + 1];
    int *d = new int[stringALength + 1];

    for (int i = 0; i <= stringALength; ++i)
    {
        p[i] = i;
    }

    int cost;
    QChar currentStringBCharacter;

    for (int j = 1; j <= stringBLength; ++j)
    {
        currentStringBCharacter = stringB.at(j - 1);
        d[0] = j;

        for (int i = 1; i <= stringALength; ++i)
        {
            cost = stringA.at(i - 1) == currentStringBCharacter ? 0 : 1;
            d[i] = qMin(qMin(d[i - 1] + gap, p[i] + gap), p[i - 1] + cost);
        }
        qSwap(p, d);
    }

    int min = qMin(stringALength, stringBLength);
    int difference = qMax(stringALength, stringBLength) - min;

    double result = (double)p[stringALength] / (double)(min + difference * gap);
    delete[] p;
    delete[] d;
    return result;
}

} // namespace

namespace udg {

Patient::Patient(QObject *parent)
 : QObject(parent)
{
    m_databaseID = 0;
}

Patient::Patient(const Patient &patient, QObject *parent)
 : QObject(parent)
{
    m_databaseID = patient.m_databaseID;
    m_fullName = patient.m_fullName;
    m_patientID = patient.m_patientID;
    m_birthDate = patient.m_birthDate;
    m_sex = patient.m_sex;
    m_identityIsRemoved = patient.m_identityIsRemoved;
    m_studiesList = patient.m_studiesList;
}

Patient::~Patient()
{
    m_studiesList.clear();
}

void Patient::setFullName(const QString &name)
{
    m_fullName = name;
}

QString Patient::getFullName() const
{
    return m_fullName;
}

void Patient::setID(const QString &id)
{
    m_patientID = id;
}

QString Patient::getID() const
{
    return m_patientID;
}

void Patient::setDatabaseID(qlonglong databaseID)
{
    m_databaseID = databaseID;
}

qlonglong Patient::getDatabaseID() const
{
    return m_databaseID;
}

void Patient::setBirthDate(int day, int month, int year)
{
    m_birthDate.setDate(year, month, day);
}

void Patient::setBirthDate(const QString &date)
{
    m_birthDate = QDate::fromString(QString(date).remove("."), "yyyyMMdd");
}

QString Patient::getBirthDateAsString()
{
    return m_birthDate.toString(Qt::LocaleDate);
}

QDate Patient::getBirthDate() const
{
    return m_birthDate;
}

int Patient::getDayOfBirth()
{
    return m_birthDate.day();
}

int Patient::getMonthOfBirth()
{
    return m_birthDate.month();
}

int Patient::getYearOfBirth()
{
    return m_birthDate.year();
}

void Patient::setSex(const QString &sex)
{
    m_sex = sex;
}

QString Patient::getSex() const
{
    return m_sex;
}

bool Patient::addStudy(Study *study)
{
    const QString &uid = study->getInstanceUID();

    if (uid.isEmpty())
    {
        ERROR_LOG("Study has empty Study Instance UID. It can't be added to the patient.");
        return false;
    }
    else if (this->studyExists(uid))
    {
        DEBUG_LOG(QString("There is already another study with the same Study Instance UID: %1. This one won't be added to the patient.").arg(uid));
        return false;
    }
    else
    {
        study->setParentPatient(this);
        this->insertStudy(study);
        emit studyAdded(study);
        return true;
    }
}

void Patient::removeStudy(const QString &uid)
{
    int index = this->findStudyIndex(uid);
    if (index != -1)
    {
        Study *removedStudy = m_studiesList.takeAt(index);
        emit studyRemoved(removedStudy);
    }
}

Study *Patient::getStudy(const QString &uid)
{
    int index = this->findStudyIndex(uid);
    if (index != -1)
    {
        return m_studiesList[index];
    }
    else
    {
        return NULL;
    }
}

bool Patient::studyExists(const QString &uid)
{
    if (this->findStudyIndex(uid) != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Patient::getNumberOfStudies()
{
    return m_studiesList.size();
}

QList<Study*> Patient::getStudies(Study::StudySortType sortCriteria) const
{
    if (sortCriteria == Study::OlderStudiesFirst)
    {
        return Study::sortStudies(m_studiesList, Study::OlderStudiesFirst);
    }
    else
    {
        return m_studiesList;
    }
}

QList<Study*> Patient::getStudiesByModality(const QString &modality)
{
    QList<Study*> filteredStudies;

    foreach (Study *study, m_studiesList)
    {
        if (study->getModalities().contains(modality))
        {
            filteredStudies << study;
        }
    }
    
    return filteredStudies;
}

QStringList Patient::getModalities() const
{
    QStringList modalities;

    foreach (Study *study, m_studiesList)
    {
        modalities.append(study->getModalities());
    }
    
    modalities.removeDuplicates();

    return modalities;
}

Series *Patient::getSeries(const QString &uid)
{
    Series *result = NULL;
    foreach (Study *study, m_studiesList)
    {
        result = study->getSeries(uid);
        if (result)
        {
            break;
        }
    }
    return result;
}

QList<Series*> Patient::getSelectedSeries()
{
    QList<Series*> selectedSeries;
    foreach (Study *study, m_studiesList)
    {
        QList<Series*> list = study->getSelectedSeries();
        if (!list.empty())
        {
            selectedSeries << list;
        }
    }
    return selectedSeries;
}

QList<Volume*> Patient::getVolumesList()
{
    QList<Volume*> volumesList;
    foreach (Study* study, m_studiesList)
    {
        volumesList << study->getVolumesList();
    }
    return volumesList;
}

int Patient::getNumberOfVolumes()
{
    int sum = 0;
    foreach (Study* study, m_studiesList)
    {
        sum += study->getNumberOfVolumes();
    }
    return sum;
}


bool Patient::hasFile(const QString &filename)
{
    QList<Study*> studyList = this->getStudies();
    foreach (Study *study, studyList)
    {
        QList<Series*> seriesList = study->getSeries();
        foreach (Series *series, seriesList)
        {
            if (series->getImagesPathList().contains(filename))
            {
                return true;
            }
        }
    }
    return false;
}

Patient& Patient::operator =(const Patient &patient)
{
    m_fullName = patient.m_fullName;
    m_patientID = patient.m_patientID;
    m_birthDate = patient.m_birthDate;
    m_sex = patient.m_sex;
    m_identityIsRemoved = patient.m_identityIsRemoved;
    m_studiesList = patient.m_studiesList;
    return *this;
}

void Patient::patientFusionLogMessage(const Patient &patient)
{
    switch (compareTo(&patient))
    {
        case SamePatients:
            INFO_LOG("Fusionem dos pacients iguals: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName);
            break;
        case SamePatientIDsDifferentPatientNames:
            INFO_LOG("Fusionem dos pacients amb IDs iguals i noms diferents: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName);
            break;
        case SamePatientNamesDifferentPatientIDs:
            INFO_LOG("Fusionem dos pacients amb noms iguals i IDs diferents: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName);
            break;
        case DifferentPatients:
            INFO_LOG("Fusionem dos pacients diferents: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName);
            break;
    }
}

Patient Patient::operator +(const Patient &patient)
{
    Patient result;
    patientFusionLogMessage(patient);
    // Copiem informació estructural en el resultat
    result.copyPatientInformation(&patient);

    result.m_studiesList = this->m_studiesList;

    // Ara recorrem els estudis que té "l'altre pacient" per afegir-los al resultat si no els té ja
    QList<Study*> studyListToAdd = patient.getStudies();
    foreach (Study *study, studyListToAdd)
    {
        // TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient!
        // Potser caldria fer una copia de l'study
        result.addStudy(study);
    }

    return result;
}

Patient Patient::operator +=(const Patient &patient)
{
    // Deixem el criteri de fusionar en mans de qui "fusioni els pacients", de totes formes fem log per
    // poder fer estadístiques del tema de fusió
    // en tots casos, la informació del pacient "original" és la que preval i mai es matxaca.
    patientFusionLogMessage(patient);

    // Recorrem els estudis que té "l'altre pacient" per afegir-los al resultat (aquesta mateixa instància) si no els té ja
    QList<Study*> studyListToAdd = patient.getStudies();
    QString uid;
    foreach (Study *study, studyListToAdd)
    {
        uid = study->getInstanceUID();
        if (!this->studyExists(uid))
        {
            // TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient!
            // Potser caldria fer una copia de l'study
            this->addStudy(study);
        }
        else
        {
            // És el mateix estudi, però podria ser que tingués sèries noves
            // Obtenim les series actuals
            QList<Series*> seriesList = study->getSeries();
            foreach (Series *series, seriesList)
            {
                // Si la sèrie no existeix actualment, l'afegim
                if (!this->getStudy(uid)->seriesExists(series->getInstanceUID()))
                {
                    this->getStudy(uid)->addSeries(series);
                }
            }
        }
    }

    return this;
}

QString Patient::getCleanName() const
{
    return this->getFullName().toUpper().replace(QRegExp("[^A-Z\\d]"), " ").trimmed();
}

Patient::PatientsSimilarity Patient::compareTo(const Patient *patient) const
{
    if (!patient)
    {
        DEBUG_LOG("El pacient és NUL");
        return Patient::DifferentPatients;
    }
    // Si tenen el mateix ID de pacient ja podem dir que són el mateix i no cal mirar res més.
    bool samePatientIDs = patient->m_patientID == this->m_patientID;

    // Pre-tractament sobre el nom del pacient per treure caràcters extranys
    QString nameOfThis = this->getCleanName();
    QString nameOfParameter = patient->getCleanName();

    // Mirem si tractant els caràcters extranys i canviant-los per espais els podem considerà iguals.
    bool samePatientNames = computeStringEditDistanceMetric(nameOfThis, nameOfParameter, NeedlemanWunschDistance) < 0.15;

    if (samePatientIDs && samePatientNames)
    {
        return Patient::SamePatients;
    }
    else if (samePatientIDs && !samePatientNames)
    {
        return Patient::SamePatientIDsDifferentPatientNames;
    }
    else if (!samePatientIDs && samePatientNames)
    {
        return Patient::SamePatientNamesDifferentPatientIDs;
    }
    else
    {
        return Patient::DifferentPatients;
    }
}

QString Patient::toString() const
{
    QString result;

    result += "- Patient\n";
    result += "    FullName : " + getFullName() + "\n";
    result += "    ID : " + getID() + "\n";

    foreach (Study *study, getStudies())
    {
        result += study->toString();
    }

    return result;
}

void Patient::setSelectedSeries(const QString &selectedSeriesUID)
{
    Series *selectedSeries = this->getSeries(selectedSeriesUID);
    if (selectedSeries)
    {
        selectedSeries->select();
    }
    else
    {
        QList<Study*> studyList = this->getStudies();
        if (!studyList.isEmpty())
        {
            QList<Series*> seriesList = studyList.first()->getSeries();
            if (!seriesList.isEmpty())
            {
                seriesList.first()->select();
            }
        }
    }
}
void Patient::copyPatientInformation(const Patient *patient)
{
    this->m_fullName = patient->m_fullName;
    this->m_patientID = patient->m_patientID;
    this->m_birthDate = patient->m_birthDate;
    this->m_sex = patient->m_sex;
}

void Patient::insertStudy(Study *study)
{
    int i = 0;
    while (i < m_studiesList.size() && m_studiesList.at(i)->getDateTime() > study->getDateTime())
    {
        ++i;
    }
    m_studiesList.insert(i, study);
}

int Patient::findStudyIndex(const QString &uid)
{
    int i = 0;
    bool found = false;
    while (i < m_studiesList.size() && !found)
    {
        if (m_studiesList.at(i)->getInstanceUID() == uid)
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
