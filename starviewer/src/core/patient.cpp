/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patient.h"
#include "logging.h"

#include <QStringList>

#include "image.h" //TODO Treure quan es refaci el toString
#include "series.h" //TODO Treure quan es refaci el toString

namespace udg {

Patient::Patient(QObject *parent)
 : QObject( parent )
{
}

Patient::~Patient()
{
    m_studiesSet.clear();
}

void Patient::setFullName( QString name )
{
    m_fullName = name;
}

void Patient::setID( QString id )
{
    m_patientID = id;
}

void Patient::setBirthDate( int day , int month , int year )
{
    m_birthDate.setYMD( year , month , day );
}

void Patient::setBirthDate( QString date )
{
    QStringList split = date.split("/");
    if( split.size() == 3 )
        this->setBirthDate( split.at(0).toInt(), split.at(1).toInt(), split.at(2).toInt() );
    else
        DEBUG_LOG( "Format de data incorrecte. Format esperat: dd/mm/yyyy. Això és el que hi havia: " + date );
}

QString Patient::getBirthDate()
{
    return m_birthDate.toString( "dd/MM/yyyy" );
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

void Patient::setSex( QString sex )
{
    m_sex = sex;
}

bool Patient::addStudy( Study *study )
{
    bool ok = true;
    QString uid = study->getInstanceUID();
    if( uid.isEmpty() )
    {
        ok = false;
        DEBUG_LOG("L'uid de l'estudi està buit! No el podem insertar per inconsistent");
    }
    else if( m_studiesSet.contains( uid ) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix un estudi amb aquest mateix UID:: " + uid );
    }
    else
    {
        m_studiesSet[ uid ] = study;
        study->setParentPatient( this );
    }

    return ok;
}

void Patient::removeStudy( QString uid )
{
    m_studiesSet.remove( uid );
}

Study *Patient::getStudy( QString uid )
{
    if( m_studiesSet.contains( uid ) )
        return m_studiesSet[ uid ];
    else
        return 0;
}

bool Patient::studyExists( QString uid )
{
    if( this->getStudy(uid) )
        return true;
    else
        return false;
}

int Patient::getNumberOfStudies()
{
    return m_studiesSet.size();
}

QList<Study*> Patient::getStudies() const
{
    return m_studiesSet.values();
}

Patient *Patient::operator +( const Patient *patient )
{
    Patient *result = new Patient;
    if( this->isSamePatient( patient ) )
    {
        // copiem informació estructural en el resultat
        result->copyPatientInformation( patient );
        result->m_studiesSet = this->m_studiesSet;

        // ara recorrem els estudis que té "l'altre pacient" per afegir-los al resultat si no els té ja
        QList<Study *> studyListToAdd = patient->getStudies();
        QString uid;
        foreach( Study *study, studyListToAdd )
        {
            uid = study->getInstanceUID();
            if( !result->studyExists(uid) )
                result->addStudy( study ); //\TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient! Potser caldria fer una copia de l'study
        }
    }
    else
    {
        DEBUG_LOG("Els pacients no es poden fusionar perquè no s'identifiquen com el mateix");
    }
    return result;
}

Patient *Patient::operator +=( const Patient *patient )
{
    // si coincideix nom o ID llavors es poden fusionar TODO mirar de definir aquest criteri
    if( isSamePatient( patient ) )
    {
        // recorrem els estudis que té "l'altre pacient" per afegir-los al resultat (aquesta mateixa instància) si no els té ja
        QList<Study *> studyListToAdd = patient->getStudies();
        QString uid;
        foreach( Study *study, studyListToAdd )
        {
            uid = study->getInstanceUID();
            if( !this->studyExists(uid) )
                this->addStudy( study ); //\TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient! Potser caldria fer una copia de l'study
        }
    }
    else
    {
        DEBUG_LOG("Els pacients no es poden fusionar perquè no comparteixen ni nom ni ID");
    }
    return this;
}

Patient *Patient::operator -( const Patient *patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

Patient *Patient::operator -=( const Patient *patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

bool Patient::isSamePatient( const Patient *patient )
{
    // si coincideix nom o ID llavors es poden considerar que són el mateix pacient TODO mirar de definir aquest criteri
    return patient->m_fullName == this->m_fullName || patient->m_patientID == this->m_patientID;
}

QString Patient::toString()
{
    QString result;

    result += "- Patient\n";
    result += "    FullName : " + getFullName() + "\n";
    result += "    ID : " + getID() + "\n";

    foreach (Study *study, getStudies())
    {
        result += "    - Study\n";
        result += "        InstanceUID : " + study->getInstanceUID() + "\n";
        result += "        Description : " + study->getDescription() + "\n";

        foreach (Series *series, study->getSeries())
        {
            result += "        - Series\n";
            result += "            SeriesNumber : " + series->getSeriesNumber() + "\n";
            result += "            Modality : " + series->getModality() + "\n";
            result += "            Description : " + series->getDescription() + "\n";
            result += "            ProtocolName : " + series->getProtocolName() + "\n";

            if (series->hasImages())
            {
                foreach (Image *image, series->getImages())
                {
                    result += "            - Image " + image->getPath() + "\n";
                }
            }
            else
            {
                result += "            - No té imatges\n";
            }
/*
            if (series->hasPresentationStates())
            {
                ....
            }
            else
            {
                result += "No té PresentationStates";
            }
            result += "\n";
*/
        }
    }

    return result;
}

void Patient::copyPatientInformation( const Patient *patient )
{
    this->m_fullName = patient->m_fullName;
    this->m_patientID = patient->m_patientID;
    this->m_birthDate = patient->m_birthDate;
    this->m_sex = patient->m_sex;
}

}
