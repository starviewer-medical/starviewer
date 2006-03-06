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

/** Afegeix el Path d'una imatge a la sèrie
  *    @param string [in] path de la imatge
  */
void SeriesVolum::addImage(std::string path)
{
    m_vectorSeriesVolum.push_back(path);
}

/** retorna el nombre d'imatges de la sèrie
  *    @return número d'imatges de les series
  */
int SeriesVolum::getNumberOfImages()
{
    return m_vectorSeriesVolum.size();
}

/** Estableix el UID de l'estudi al que pertany la serie
  *    @param string [in] UID de l'estudi
  */
void SeriesVolum::setStudyUID(std::string UID)
{
    m_studyUID = UID;
}

/** Estableix el UID de la sèrie al que pertany la serie
  *    @param string [in] UID de la sèrie
  */
void SeriesVolum::setSeriesUID(std::string UID)
{
    m_seriesUID = UID;
}

/** Estableix el Id de l'estudi al que pertany la serie
  *    @param string [in] Id de l'estudi
  */
void SeriesVolum::setStudyId(std::string Id)
{
    m_studyId = Id;
}

/** Estableix l'Id de la serie
  *    @param string [in] Ud de la serie
  */
void SeriesVolum::setSeriesId(std::string Id)
{
    m_seriesId = Id;
}

/** Estableix la modalitat de la serie
  *    @param string [in] modalitat de la serie
  */
void SeriesVolum::setSeriesModality(std::string modality)
{
    m_seriesModality = modality;
}

/** Estableix el path d'una serie
  *    @param string [in] path de la serie
  */
void SeriesVolum::setSeriesPath(std::string path)
{
    m_seriesPath = path;
}

/** retorna el UID de la serie
  *    @return UID de la serie
  */
std::string SeriesVolum::getSeriesUID()
{
    return m_seriesUID;
}

/** retorna el UID de l'estudi
  *    @return UID de l'estudi
  */
std::string SeriesVolum::getStudyUID()
{
    return m_studyUID;
}

/** retorna el Id de la serie
  *    @return Id de la serie
  */
std::string SeriesVolum::getSeriesId()
{
    return m_seriesId;
}

/** retorna el Id de l'estudi
  *    @return Id de l'estudi
  */
std::string SeriesVolum::getStudyId()
{
    return m_studyId;
}

/** retorna la modalitat de la serie
  *    @return modalitat de la serie
  */
std::string SeriesVolum::getSeriesModality()
{
    return m_seriesModality;
}

/** retorna el path de la serie
  *    @return el path de la serie
  */
std::string SeriesVolum::getSeriesPath()
{
    return m_seriesPath;
}

SeriesVolum::~SeriesVolum()
{

}

};
