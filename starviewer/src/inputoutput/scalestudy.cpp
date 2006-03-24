/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "scalestudy.h"
#include "serieslist.h"
#include "seriesmask.h"
#include "cachepacs.h"
#include "status.h"
#include "imagemask.h"
#include "image.h"
#include "imagelist.h"
#include "starviewersettings.h"
#include "scaleimage.h"

namespace udg {

ScaleStudy::ScaleStudy()
{
}

/** Escala una imatge de cada sèrie per poder fer la previsualització a la caché local del studyUID 
  *         @param Uid de l'estudi a escakar
  */
void ScaleStudy::scale(std::string studyUID)
{
    Status state;
    SeriesList seriesList;
    ImageMask mask;
    int number;
    char imgNumX[6];
    std::string absPath,relPath,absPathScal;
    StarviewerSettings settings;
    ScaleImage scaleImg;
    state = getSeriesOfStudy(studyUID,seriesList); //busquem les sèries de l'estudi
    
    seriesList.firstSeries();
    
    while (!seriesList.end()) //escalem una imatge per cada sèrie
    {  
        //preparem la màscara per buscar la imatge del mig de l'estudi
        mask.setSeriesUID(seriesList.getSeries().getSeriesUID().c_str());
        mask.setStudyUID(studyUID.c_str());    
        state = countImageNumber(mask, number);//comptem el número d'imatges, per saber quina és la imatge del mig
        
        sprintf(imgNumX,"%i",number/2 + 1);
        mask.setImageNumber(imgNumX);//indiquem per quin número d' imatge hem de buscar el path
        
        relPath.clear();
        imageRelativePath(mask,relPath);//busquem el path de la imatge del mig
        
        if (relPath.length() == 0)
        {/*hi ha algunes màquines per error tenen el número d'imatge 1 sempre, encara que l'estudi tingui 10 imatges
          *En aquest cas no funciona buscar la imatge del mig, i el que es fa és mostrar la primera imatge que trobem
          */
            mask.setImageNumber("");
            imageRelativePath(mask,relPath);
        }
        absPath.clear();
        absPath.append(settings.getCacheImagePath().toAscii().constData());
        absPath.append(relPath); //creem el path absolut a la imatge a la imatge
        
        //creem el nom de la imatge resultant escalada
        absPathScal.clear();
        absPathScal.append(settings.getCacheImagePath().toAscii().constData());
        absPathScal.append(studyUID);
        absPathScal.append("/");
        absPathScal.append(seriesList.getSeries().getSeriesUID().c_str());
        absPathScal.append("/");
        absPathScal.append("scaled.jpeg");
        
        scaleImg.dicom2lpgm(absPath.c_str(),absPathScal.c_str(),100);//creem la imatge escalada
        
        seriesList.nextSeries();     
            
    }
   
}

/** Cerca les series de l'estudi
  *        @param El UID de l'estudi a cercar
  *        @param [out] retorna les sèries de l'estudi
  *        @return retorna l'estat del mètode
  */
Status ScaleStudy::getSeriesOfStudy(std::string studyUID,SeriesList &seriesList)
{
    SeriesMask mask;
    CachePacs *localCache = CachePacs::getCachePacs();
    
    mask.setStudyUID(studyUID.c_str());
    
    return localCache->querySeries(mask,seriesList);
}

/** Compta el número d'imatges de l'estudi
  *        @param màscara de les imatges a comptar
  *        @param [out]  número d'imatges
  *        @return retorna l'estat del mètode
  */
Status ScaleStudy::countImageNumber(ImageMask mask,int &number)
{
    CachePacs *localCache = CachePacs::getCachePacs(); 
    
    return localCache->countImageNumber(mask,number);
}

/** busca el path retatiu de la imatge que se li passa per paràmetre
  *        @param màscara de la imatge a buscar el path relatiu
  *        @param [out]  path relatiu, respecte el directori de l'estudi
  *        @return retorna l'estat del mètode
  */
Status ScaleStudy::imageRelativePath(ImageMask mask,std::string &relPath)
{
    ImageList imageList;
    Image image;
    Status state;
    
    CachePacs *localCache = CachePacs::getCachePacs();

    state = localCache->queryImages(mask,imageList);
    
    imageList.firstImage();
    if (!imageList.end())
    {
        image = imageList.getImage();
        relPath.clear();
        relPath.append(image.getStudyUID());
        relPath.append("/");
        relPath.append(image.getSeriesUID());
        relPath.append("/");
        relPath.append(image.getImageName());
    }
    return state;
}


ScaleStudy::~ScaleStudy()
{
}


};
