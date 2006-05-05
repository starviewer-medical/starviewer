/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSTUDYVOLUM_H
#define UDGSTUDYVOLUM_H

#include <vector>
#include <string> 
#include "seriesvolum.h"

namespace udg {

/** Aquesta classe és permet organitzar els estudis per a que l'starviewer els pugui visualitzar
@author marc
*/
class StudyVolum{

private: 

    std::string m_studyUID;
    std::string m_studyId;
    std::string m_studyTime;
    std::string m_studyDate;
    std::string m_patientName;
    std::string m_patientId;
    std::string m_patientAge;
    std::string m_defaultSeriesUID;//conté el UID de la serie que s'ha visualitzar per defecte, ja que quant visualitzem només s'obrira una sèrie de l'estudi
    
    std::vector <SeriesVolum> m_vectorStudyVolum;
    std::vector <SeriesVolum>::iterator i;

public:
    StudyVolum();
    
    void addSeriesVolum(SeriesVolum);
    int getNumberOfSeries();

    void setStudyUID(std::string);
    void setStudyId(std::string);
    void setStudyTime(std::string);
    void setStudyDate(std::string);
    void setPatientName(std::string);
    void setPatientId(std::string);
    void setPatientAge(std::string);
    void setDefaultSeriesUID(std::string);
    
    std::string getStudyUID();
    std::string getStudyId();
    std::string getStudyTime();
    std::string getStudyDate();
    std::string getPatientName();
    std::string getPatientId();
    std::string getPatientAge();
    std::string getDefaultSeriesUID();
    
    void firstSerie();
    void nextSerie();
    bool end();
    
    SeriesVolum getSeriesVolum();
    
    ~StudyVolum();

};

};

#endif
