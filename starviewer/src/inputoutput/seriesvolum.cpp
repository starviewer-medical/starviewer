/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "seriesvolum.h"

namespace udg {

SeriesVolum::SeriesVolum()
{
}

SeriesVolum::~SeriesVolum()
{
}

void SeriesVolum::addImage( QString imagePath )
{
    m_seriesImagePathList += imagePath;
}

int SeriesVolum::getNumberOfImages()
{
    return m_seriesImagePathList.size();
}

void SeriesVolum::setStudyUID( QString UID )
{
    m_studyUID = UID;
}

void SeriesVolum::setSeriesUID( QString UID )
{
    m_seriesUID = UID;
}

void SeriesVolum::setStudyId( QString Id )
{
    m_studyId = Id;
}

void SeriesVolum::setSeriesId( QString Id )
{
    m_seriesId = Id;
}

void SeriesVolum::setSeriesModality( QString modality )
{
    m_seriesModality = modality;
}

void SeriesVolum::setSeriesPath( QString path )
{
    m_seriesPath = path;
}

QString SeriesVolum::getSeriesUID()
{
    return m_seriesUID;
}

QString SeriesVolum::getStudyUID()
{
    return m_studyUID;
}

QString SeriesVolum::getSeriesId()
{
    return m_seriesId;
}

QString SeriesVolum::getStudyId()
{
    return m_studyId;
}

QString SeriesVolum::getSeriesModality()
{
    return m_seriesModality;
}

QString SeriesVolum::getSeriesPath()
{
    return m_seriesPath;
}

QStringList SeriesVolum::getImagesPathList()
{
	return m_seriesImagePathList;
}

};
