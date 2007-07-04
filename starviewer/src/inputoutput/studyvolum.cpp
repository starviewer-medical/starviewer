/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "studyvolum.h"
#include "logging.h"

namespace udg {

StudyVolum::StudyVolum()
{
}

void StudyVolum::addSeriesVolum( SeriesVolum seriesVolum )
{
    m_studyVolumList += seriesVolum;
}

int StudyVolum::getNumberOfSeries()
{
    return m_studyVolumList.size();
}

void StudyVolum::setStudyUID( QString studyUID )
{
    m_studyUID = studyUID;
}

void StudyVolum::setStudyId( QString studyId )
{
    m_studyId = studyId;
}

void StudyVolum::setStudyTime( QString time )
{
    m_studyTime = time;
}

void StudyVolum::setStudyDate( QString date )
{
    m_studyDate = date;
}

void StudyVolum::setPatientName( QString name )
{
    m_patientName = name;
}

void StudyVolum::setPatientId( QString patientId )
{
    m_patientId = patientId;
}

void StudyVolum::setPatientAge( QString age )
{
    m_patientAge = age;
}

void StudyVolum::setDefaultSeriesUID( QString seriesUID )
{
    m_defaultSeriesUID = seriesUID;
}

QString StudyVolum::getStudyUID()
{
    return m_studyUID;
}

QString StudyVolum::getStudyId()
{
    return m_studyId;
}

QString StudyVolum::getStudyTime()
{
    return m_studyTime;
}

QString StudyVolum::getStudyDate()
{
    return m_studyDate;
}

QString StudyVolum::getPatientName()
{
    return m_patientName;
}

QString StudyVolum::getPatientId()
{
    return m_patientId;
}

QString StudyVolum::getPatientAge()
{
    return m_patientAge;
}

QString StudyVolum::getDefaultSeriesUID()
{
    return m_defaultSeriesUID;
}

SeriesVolum StudyVolum::getSeriesVolum( int index )
{
    SeriesVolum result;
    if( index > m_studyVolumList.size() )
    {
        DEBUG_LOG("S'està demanant un SeriesVolum no disponible. Índex > que la mida de la llista de sèries disponibles");
    }
    else
    {
        if ( index >= 0 )
            result = m_studyVolumList.at( index );
    }
    return result;
}

StudyVolum::~StudyVolum()
{
}

};
