/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGIMAGE_H
#define UDGIMAGE_H

#include <string>

namespace udg {

/** Classe image encarregada de guardar la informació d'una imatge
@author marc
*/
class Image{
public:
    Image();
   ~Image();

   void setSoPUID (std::string );
   void setSeriesUID (std::string );
   void setImagePath (std::string );
   void setStudyUID (std::string );
   void setImageName (std::string );
   void setImageNumber (int);
   void setImageSize (int);
   
   std::string getSoPUID();
   std::string getSeriesUID();
   std::string getImagePath ();
   std::string getStudyUID();
   std::string getImageName();
   int getImageNumber();
   int getImageRows();
   int getImageSize();
       
private:

    std::string m_SoPUID;
    std::string m_seriesUID;
    std::string m_imagePath;
    std::string m_studyUID;
    std::string m_imageName;
    int m_imageNumber;
    int m_imageSize;

};

};

#endif
