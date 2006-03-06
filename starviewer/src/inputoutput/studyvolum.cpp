/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "studyvolum.h"

namespace udg {

StudyVolum::StudyVolum()
{
}

/** Permet afegir una sèrie a aquest estudi. S'utilitza per especificar quines sèries te un estudi
  *    @param SeriesVolum SeriesVolum a afegir
  */
void StudyVolum::addSeriesVolum(SeriesVolum Volum)
{
    m_vectorStudyVolum.push_back(Volum);
}

/** Retorna el número de series que conte l'estudi
  *    @return retorna el numero de series de l'estudi
  */
int StudyVolum::getNumberOfSeries()
{
    return m_vectorStudyVolum.size();
}

/** Estableix el UID de l'estudi
  *    @param UID de l'estudi
  */
void StudyVolum::setStudyUID(std::string UID)
{
    m_studyUID=UID;
}

/** Estableix el Id de l'estudi
  *    @param Id de l'estudi
  */
void StudyVolum::setStudyId(std::string Id)
{
    m_studyId=Id;
}

/** Estableix el temps en que es va realitzar l'estudi
  *    @param Hora en que es va realitzar l'estudi
  */
void StudyVolum::setStudyTime(std::string time)
{
    m_studyTime = time;
}

/** Estableix la data en que es va realitzar l'estudi
  *    @param data de l'estudi
  */
void StudyVolum::setStudyDate(std::string date)
{
    m_studyDate = date;
}

/** Estableix el nom del pacient de l'estudi
  *    @param nom del pacient
  */
void StudyVolum::setPatientName(std::string name)
{
    m_patientName = name;
}

/** Estableix el Id del pacient
  *    @param Id del pacient
  */
void StudyVolum::setPatientId(std::string Id)
{
    m_patientId = Id;
}

/** Estableix l'edat del pacient al realitzar-li l'estudi
  *    @param Edat del pacient al realtizar-se de l'estudi
  */
void StudyVolum::setPatientAge(std::string age)
{
    m_patientAge = age;
}

/** Estableix quina sera la serie per defecte s'obrira a l'starviewer al visualitzar l'estudi
  *    @param UID de la serie a visualitzar
  */
void StudyVolum::setDefaultSeriesUID(std::string seriesUID)
{
    m_defaultSeriesUID = seriesUID;
}

/** retorna el UID de l'estudi
  *    @return UID de l'estudi
  */
std::string StudyVolum::getStudyUID()
{
    return m_studyUID;
}

/** retorna el Id de l'estudi
  *    @return Id de l'estudi
  */
std::string StudyVolum::getStudyId()
{
    return m_studyId;
}

/** retorna el hora en que s'ha realitzat l'estudi
  *    @return hora de l'estudi
  */
std::string StudyVolum::getStudyTime()
{
    return m_studyTime;
}

/** retorna la data en que s'ha realitzat l'estudi
  *    @return data de l'estudi
  */
std::string StudyVolum::getStudyDate()
{
    return m_studyDate;
}

/** retorna el nom del pacient a qui s'ha realitzat l'estudi
  *    @return nom del pacient
  */
std::string StudyVolum::getPatientName()
{
    return m_patientName;
}

/** retorna el Id del pacient a qui s'ha realitzat l'estudi
  *    @return Id del pacient
  */
std::string StudyVolum::getPatientId()
{
    return m_patientId;
}

/** retorna el l'edat del pacient al realitzar-li l'estudi
  *    @return edat del pacient
  */
std::string StudyVolum::getPatientAge()
{
    return m_patientAge;
}

/** retorna la serie que s'ha d'obrir de l'estudi en el visualitzador per defecte
  *    @return UID de la sèrie a visualitzar
  */
std::string StudyVolum::getDefaultSeriesUID()
{
    return m_defaultSeriesUID;
}

/** Es situa a la primera serie de l'estudi
  */
void StudyVolum::firstSerie()
{
    i = m_vectorStudyVolum.begin();
}    

/** seguent serie
  */
void StudyVolum::nextSerie()
{
    i++;
}

/** indica si s'ha arribat al final de la llista
  *    @return boolea indicant si hem arribat al final de la llista de series
  */
bool StudyVolum::end()
{
    return i == m_vectorStudyVolum.end();
}

/** retorna l'actual seriesVolum al que apunta la llista
  *    @return retorna el seriesVolum al que s'apunta
  */
SeriesVolum StudyVolum::getSeriesVolum()
{
    return (*i);
}

StudyVolum::~StudyVolum()
{
}


};
