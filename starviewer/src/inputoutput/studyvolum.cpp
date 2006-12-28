/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include <string>

#include "studyvolum.h"

namespace udg {

StudyVolum::StudyVolum()
{
}

void StudyVolum::addSeriesVolum( SeriesVolum Volum )
{
    m_vectorStudyVolum.push_back( Volum );
}

int StudyVolum::getNumberOfSeries()
{
    return m_vectorStudyVolum.size();
}

void StudyVolum::setStudyUID( std::string UID )
{
    m_studyUID=UID;
}

void StudyVolum::setStudyId( std::string Id )
{
    m_studyId=Id;
}

void StudyVolum::setStudyTime( std::string time )
{
    m_studyTime = time;
}

void StudyVolum::setStudyDate( std::string date )
{
    m_studyDate = date;
}

void StudyVolum::setPatientName( std::string name )
{
    m_patientName = name;
}

void StudyVolum::setPatientId( std::string Id )
{
    m_patientId = Id;
}

void StudyVolum::setPatientAge( std::string age )
{
    m_patientAge = age;
}

void StudyVolum::setDefaultSeriesUID( std::string seriesUID )
{
    m_defaultSeriesUID = seriesUID;
}

std::string StudyVolum::getStudyUID()
{
    return m_studyUID;
}

std::string StudyVolum::getStudyId()
{
    return m_studyId;
}

std::string StudyVolum::getStudyTime()
{
    return m_studyTime;
}

std::string StudyVolum::getStudyDate()
{
    return m_studyDate;
}

std::string StudyVolum::getPatientName()
{
    return m_patientName;
}

std::string StudyVolum::getPatientId()
{
    return m_patientId;
}

std::string StudyVolum::getPatientAge()
{
    return m_patientAge;
}

std::string StudyVolum::getDefaultSeriesUID()
{
    return m_defaultSeriesUID;
}

void StudyVolum::firstSerie()
{
    i = m_vectorStudyVolum.begin();
}    

void StudyVolum::nextSerie()
{
    i++;
}

bool StudyVolum::end()
{
    return i == m_vectorStudyVolum.end();
}

SeriesVolum StudyVolum::getSeriesVolum()
{
    return (*i);
}

StudyVolum::~StudyVolum()
{
}

};
