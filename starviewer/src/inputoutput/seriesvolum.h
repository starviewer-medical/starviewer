/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSERIESVOLUM_H
#define UDGSERIESVOLUM_H

#include <vector.h>
#include <string.h>

namespace udg {

/** Permet definir una sèrie d'un estudi, i indicar el path on es troben les imatges en el disc local
@author marc
*/
class SeriesVolum{

private:

    std::string m_seriesUID;
    std::string m_studyUID;
    std::string m_studyId;
    std::string m_seriesId;
    std::string m_seriesModality;
    std::string m_seriesPath;

    vector <std::string> m_vectorSeriesVolum;
        
public:

    SeriesVolum();

    void addImage(std::string);
    
    void setStudyUID(std::string);
    void setSeriesUID(std::string);
    void setStudyId(std::string);
    void setSeriesId(std::string);
    void setSeriesModality(std::string);
    void setSeriesPath(std::string);
        
    std::string getSeriesUID();
    std::string getStudyUID();
    std::string getSeriesId();
    std::string getStudyId();
    std::string getSeriesModality();
    std::string getSeriesPath();
        
    int getNumberOfImages();
    
    ~SeriesVolum();

};

};

#endif
