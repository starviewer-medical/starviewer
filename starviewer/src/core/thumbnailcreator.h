/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGTHUMBNAILCREATOR_H
#define UDGTHUMBNAILCREATOR_H

class QImage;
class QPixmap;
class QString;

namespace udg {

class Series;
class Image;

class ThumbnailCreator
{
public:

    ///Crea un thumbnail a partir de les imatges de la sèrie
    QImage getThumbnail(const Series *series, int resolution = 100);

    ///Crea el thumbnail de la imatge passada per paràmetre
    QImage getThumbnail(const Image *image, int resolution = 100);

private :

    ///Crea el thumbnail d'un objecte dicom que sigui una imatge
    QImage createImageThumbnail(QString imageFileName, int resolution = 100);

};

}

#endif
