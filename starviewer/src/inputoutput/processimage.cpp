/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "processimage.h"

#include <QtGlobal>

namespace udg {

//aquesta classes està buida per poder ser reimplementada
ProcessImage::ProcessImage()
{
}

void ProcessImage::process(Image* image)
{
    qFatal("process Not implemented");
}

void ProcessImage::process(DICOMTagReader *dicomTagReader)
{
    qFatal("process Not implemented");
}

};
