/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSCALESTUDY_H
#define UDGSCALESTUDY_H

#include <string>

namespace udg {

class SeriesList;
class Status;
class ImageMask;
/** Escala les imatges del mig de cada sèrie d'un estudi, per poder ser previsualitzades quant es consulta la caché
@author Grup de Gràfics de Girona  ( GGG )
*/
class ScaleStudy{

private :

public:
    ScaleStudy();
    
    void scale(std::string studyUID);
    Status getSeriesOfStudy(std::string studyUID,SeriesList &seriesList);
    Status countImageNumber(ImageMask,int &number);
    Status imageRelativePath(ImageMask,std::string &relPath);
    ~ScaleStudy();

};

};

#endif
