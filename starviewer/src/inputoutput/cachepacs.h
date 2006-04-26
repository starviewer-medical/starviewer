/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGCACHEPACS_H
#define UDGCACHEPACS_H

#include <sqlite.h>
#include "study.h"
#include "studymask.h"
#include "seriesmask.h"
#include "series.h"
#include "imagemask.h"
#include "image.h"
#include "studylist.h"
#include "serieslist.h"
#include "imagelist.h"
#include "databaseconnection.h"

#include <string.h>

class Status;

namespace udg {

/** Classe que gestiona la caché de l'aplicació, i fa els inserts,queries i deletre
@author marc
*/
class CachePacs{

private:
    
    DatabaseConnection *m_DBConnect;
        
    CachePacs();
    ~CachePacs();
    
    Status constructState(int error);

    std::string buildSqlQueryStudy( StudyMask *studyMask );
    std::string buildSqlQuerySeries( SeriesMask *seriesMask );
    std::string buildSqlQueryImages( ImageMask *imageMask );
    std::string buildSqlCountImageNumber( ImageMask *imageMask );    
            
    int getTime();
    int getDate();
    
    std::string replaceAsterisk( std::string );

public:
 
     static CachePacs* getCachePacs()
     {
         static CachePacs cache;
         return &cache; 
     }
   
    Status insertStudy(Study *); 
    Status insertSeries(Series *); 
    Status insertImage(Image *);

    Status queryStudy(StudyMask mask,StudyList &list);
    Status queryStudy(std::string studyUID,Study &study);
    Status queryOldStudies( std::string , StudyList &list );
    Status querySeries(SeriesMask mask,SeriesList &list);
    Status queryImages(ImageMask mask,ImageList &list);
    Status countImageNumber(ImageMask mask,int &imageNumber);
    
    Status updateStudyAccTime(std::string studyUID);
    Status setStudyRetrieved(std::string studyUID);
    Status setStudyModality(std::string studyUID,std::string modality);
    Status setStudyRetrieving(std::string studyUID);
    
    Status delStudy(std::string);
    Status delNotRetrievedStudies();    
    
    Status compactCachePacs();
    
};

};

#endif
