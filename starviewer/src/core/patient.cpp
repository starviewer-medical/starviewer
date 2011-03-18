/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patient.h"
#include "logging.h"

#include <QStringList>
#include <QChar>
#include <QSet>

namespace udg {

Patient::Patient(QObject *parent)
 : QObject(parent)
{
    m_databaseID = NULL;
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
    m_studiesSet = patient.m_studiesSet;
}

Patient::~Patient()
{
    m_studiesSet.clear();
}

void Patient::setFullName(const QString &name)
{
    m_fullName = name;
}

void Patient::setID(const QString &id)
{
    m_patientID = id;
}

void Patient::setDatabaseID(qlonglong databaseID)
{
    m_databaseID = databaseID;
}

qlonglong Patient::getDatabaseID()
{
    return m_databaseID;
}

void Patient::setBirthDate(int day, int month, int year)
{
    m_birthDate.setYMD(year, month, day);
}

void Patient::setBirthDate(const QString &date)
{
    m_birthDate = QDate::fromString(QString(date).remove("."), "yyyyMMdd");
}

QString Patient::getBirthDateAsString()
{
    return m_birthDate.toString(Qt::LocaleDate);
}

QDate Patient::getBirthDate()
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

bool Patient::addStudy(Study *study)
{
    bool ok = true;
    QString uid = study->getInstanceUID();
    if (uid.isEmpty())
    {
        ok = false;
        DEBUG_LOG("L'uid de l'estudi està buit! No el podem insertar per inconsistent");
    }
    else if (this->studyExists(uid))
    {
        ok = false;
        DEBUG_LOG("Ja existeix un estudi amb aquest mateix UID:: " + uid);
    }
    else
    {
        study->setParentPatient(this);
        this->insertStudy(study);
    }

    return ok;
}

void Patient::removeStudy(const QString &uid)
{
    int index = this->findStudyIndex(uid);
    if (index != -1)
    {
        m_studiesSet.removeAt(index);
    }
}

Study *Patient::getStudy(const QString &uid)
{
    int index = this->findStudyIndex(uid);
    if (index != -1)
    {
        return m_studiesSet[index];
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
    return m_studiesSet.size();
}

QList<Study *> Patient::getStudies() const
{
    return m_studiesSet;
}

Series *Patient::getSeries(const QString &uid)
{
    Series *result = NULL;
    foreach (Study *study, m_studiesSet)
    {
        result = study->getSeries(uid);
        if (result)
        {
            break;
        }
    }
    return result;
}

QList<Series *> Patient::getSelectedSeries()
{
    QList<Series *> selectedSeries;
    foreach (Study *study, m_studiesSet)
    {
        QList<Series *> list = study->getSelectedSeries();
        if (!list.empty())
        {
            selectedSeries << list;
        }
    }
    return selectedSeries;
}

bool Patient::hasFile(const QString &filename)
{
    QList<Study *> studyList = this->getStudies();
    foreach (Study *study, studyList)
    {
        QList<Series *> seriesList = study->getSeries();
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

Patient & Patient::operator =(const Patient &patient)
{
    m_fullName = patient.m_fullName;
    m_patientID = patient.m_patientID;
    m_birthDate = patient.m_birthDate;
    m_sex = patient.m_sex;
    m_identityIsRemoved = patient.m_identityIsRemoved;
    m_studiesSet = patient.m_studiesSet;
    return *this;
}

void Patient::patientFusionLogMessage(const Patient &patient)
{
    switch (compareTo(&patient))
    {
        case SamePatients:
            INFO_LOG("Fusionem dos pacients iguals: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName);
            break;
        
        case IndeterminableSimilarity:
            INFO_LOG("Fusionem dos pacients amb similitut indeterminable: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName);
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

    result.m_studiesSet = this->m_studiesSet;

    // Ara recorrem els estudis que té "l'altre pacient" per afegir-los al resultat si no els té ja
    QList<Study *> studyListToAdd = patient.getStudies();
    foreach (Study *study, studyListToAdd)
    {
        // TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient! 
        // Potser caldria fer una copia de l'study
        result.addStudy(study);
    }

    emit patientFused();

    return result;
}

Patient Patient::operator +=(const Patient &patient)
{
    // Deixem el criteri de fusionar en mans de qui "fusioni els pacients", de totes formes fem log per
    // poder fer estadístiques del tema de fusió
    // en tots casos, la informació del pacient "original" és la que preval i mai es matxaca.
    patientFusionLogMessage(patient);

    // Recorrem els estudis que té "l'altre pacient" per afegir-los al resultat (aquesta mateixa instància) si no els té ja
    QList<Study *> studyListToAdd = patient.getStudies();
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
            // obtenim les series actuals
            QList<Series *> seriesList = study->getSeries();
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

    emit patientFused();

    return this;
}

QString Patient::clearStrangeSymbols(const QString &patientName)
{
    return patientName.toUpper().replace(QRegExp("[^A-Z ^\\d]"), " ").trimmed();
}

QString Patient::clearPatientName(const QString &patientName)
{
    return patientName.toUpper().replace(QRegExp("[^A-Z]"), " ").trimmed();
}

bool Patient::containtsNumericalSymbols(const QString &patientName)
{
    QRegExp rx("\\d\\d?\\d?\\d?");
    return (rx.indexIn(patientName) != -1);
}

Patient::PatientsSimilarity Patient::compareTo(const Patient *patient)
{
    if (!patient)
    {
        DEBUG_LOG("El pacient és NUL");
        return Patient::IndeterminableSimilarity;
    }
    // Si tenen el mateix ID de pacient ja podem dir que són el mateix i no cal mirar res més.
    if (patient->m_patientID == this->m_patientID)
    {
        return SamePatients;
    }


    // Pre-tractament sobre el nom del pacient per treure caràcters extranys
    QString nameOfThis = clearStrangeSymbols(this->getFullName());
    QString nameOfParameter = clearStrangeSymbols(patient->getFullName());

    // Tractament especial en el cas de que els noms continguin números
    if (containtsNumericalSymbols(nameOfThis) && containtsNumericalSymbols(nameOfParameter))
    {
        if (nameOfThis == nameOfParameter)
        {
            return SamePatients;
        }
        else
        {
            return DifferentPatients;
        }
    }

    // Si passem del condicional anterior és que algun o cap dels noms tenia dades numèriques; pertant fem el tractament normal.
    // mirem si tractant els caràcters extranys i canviant-los per espais són iguals. En aquest cas ja no cal mirar res més.
    if (nameOfThis == nameOfParameter)
    {
        return SamePatients;
    }

    PatientsSimilarity namesSimilarity = metricToSimilarity(computeStringEditDistanceMetric(nameOfThis, nameOfParameter, NeedlemanWunschDistance));
    if (namesSimilarity != IndeterminableSimilarity)
    {
        // Si tenen molta similitud, retornem aquest valor
        return namesSimilarity; 
    }

    return metricToSimilarity(computeStringEditDistanceMetric(patient->m_patientID, this->m_patientID, NeedlemanWunschDistance));
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

QList<Patient *> Patient::mergePatients(QList<Patient *> patientsList)
{
    QList<Patient *> resultingPatientsList;
    if (patientsList.count() == 1)
    {
        resultingPatientsList = patientsList;
    }
    else if (patientsList.count() > 1)
    {
        // El mètode no és gaire eficient perquè prova "tots contra tots"
        // ja que un cop fusionem un, en principi es podria eliminar de 
        // la llista de fusió i no ho fem i tornem a comprovar
        // TODO millorar l'algorisme de fusió dins de la llista

        QSet<Patient *> patientSet = patientsList.toSet();
        QMutableSetIterator<Patient *> setIterator(patientSet);
        while (setIterator.hasNext())
        {
            // Agafem el primer element del conjunt i el treiem
            Patient *currentPatient = setIterator.next();
            setIterator.remove();
            // Ara examinem la resta d'elements del conjunt
            // per veure si els podem fusionar
            // en cas que es puguin fusionar, els eliminarem del conjunt
            while (setIterator.hasNext())
            {
                // Comparem per fer la fusió o no amb el proper element
                if (currentPatient->compareTo(setIterator.peekNext()) == Patient::SamePatients)
                {
                    // BINGO! Són iguals! El fusionem i l'eliminem del conjunt
                    *currentPatient += (*setIterator.peekNext()) ;
                    setIterator.next();
                    setIterator.remove();   
                }
                else
                {
                    // No són iguals, no fusionem i passem al següent element del conjunt
                    setIterator.next();
                }
            }
            // Afegim a la llista el pacient ja fusionat si s'escau
            resultingPatientsList << currentPatient;
            // retornem l'iterador a l'inici per continuar analitzant la resta d'elements del conjunt que quedin
            setIterator.toFront();
        }
    }
    return resultingPatientsList;
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
        QList<Study *> studyList = this->getStudies();
        if (!studyList.isEmpty())
        {
            QList<Series *> seriesList = studyList.first()->getSeries();
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
    while (i < m_studiesSet.size() && m_studiesSet.at(i)->getDateTime() < study->getDateTime())
    {
        ++i;
    }
    m_studiesSet.insert(i, study);
}

int Patient::findStudyIndex(const QString &uid)
{
    int i = 0;
    bool found = false;
    while (i < m_studiesSet.size() && !found)
    {
        if (m_studiesSet.at(i)->getInstanceUID() == uid)
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

double Patient::computeStringEditDistanceMetric(const QString &stringA, const QString &stringB, int gap)
{
    /// Més informacíó sobre l'algorisme:
    /// http://staffwww.dcs.shef.ac.uk/people/S.Chapman/stringmetrics.html#Levenshtein
    /// http://en.wikipedia.org/wiki/Levenshtein_distance
    /// http://staffwww.dcs.shef.ac.uk/people/S.Chapman/stringmetrics.html#needleman
    /// http://en.wikipedia.org/wiki/Needleman%E2%80%93Wunsch_algorithm

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
    delete []p;
    delete []d;
    return result;
}

Patient::PatientsSimilarity Patient::metricToSimilarity(double measure)
{
    if (measure < 0.15)
    {
        return SamePatients;
    }
    else if (measure < 0.31)
    {
        return IndeterminableSimilarity;
    }
    else
    {
        return DifferentPatients;
    }
}

}
