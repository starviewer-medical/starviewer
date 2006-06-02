/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include <string>

#include "seriesvolum.h"

namespace udg {

SeriesVolum::SeriesVolum()
{
}

void SeriesVolum::addImage( std::string path )
{
    m_vectorSeriesVolum.push_back( path );
}

int SeriesVolum::getNumberOfImages()
{
    return m_vectorSeriesVolum.size();
}

void SeriesVolum::setStudyUID( std::string UID )
{
    m_studyUID = UID;
}

void SeriesVolum::setSeriesUID( std::string UID )
{
    m_seriesUID = UID;
}

void SeriesVolum::setStudyId( std::string Id )
{
    m_studyId = Id;
}

void SeriesVolum::setSeriesId( std::string Id )
{
    m_seriesId = Id;
}

void SeriesVolum::setSeriesModality( std::string modality )
{
    m_seriesModality = modality;
}

void SeriesVolum::setSeriesPath( std::string path )
{
    m_seriesPath = path;
}

std::string SeriesVolum::getSeriesUID()
{
    return m_seriesUID;
}

std::string SeriesVolum::getStudyUID()
{
    return m_studyUID;
}

std::string SeriesVolum::getSeriesId()
{
    return m_seriesId;
}

std::string SeriesVolum::getStudyId()
{
    return m_studyId;
}

std::string SeriesVolum::getSeriesModality()
{
    return m_seriesModality;
}

std::string SeriesVolum::getSeriesPath()
{
    return m_seriesPath;
}

std::vector <std::string> SeriesVolum::getVectorSeriesPath()
{
	return m_vectorSeriesVolum;
}

SeriesVolum::~SeriesVolum()
{

}

};
