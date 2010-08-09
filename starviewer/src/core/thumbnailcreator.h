/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTHUMBNAILCREATOR_H
#define UDGTHUMBNAILCREATOR_H

class QImage;
class QString;
class DicomImage;

namespace udg {

class Series;
class Image;
class DICOMTagReader;

class ThumbnailCreator {
public:
    /// Crea un thumbnail a partir de les imatges de la sèrie
    QImage getThumbnail(const Series *series, int resolution = 100);

    /// Crea el thumbnail de la imatge passada per paràmetre
    QImage getThumbnail(const Image *image, int resolution = 100);

    /// Obté el thumbnail a partir del DICOMTagReader
    QImage getThumbnail(DICOMTagReader *reader, int resolution = 100);

    /// Crea un thumbnail buit personalitzat amb el text que li donem
    static QImage makeEmptyThumbnailWithCustomText(const QString &text, int resolution = 100);

private:
    /// Crea el thumbnail d'un objecte dicom que sigui una imatge
    QImage createImageThumbnail(const QString &imageFileName, int resolution);

    /// Crea el thumbnail a partir d'un DICOMTagReader
    QImage createThumbnail(DICOMTagReader *reader, int resolution);
    
    /// Crea el thumbnail a partir d'una DicomImage
    QImage createThumbnail(DicomImage *dicomImage, int resolution);
    
    /// Comprova que el dataset compleixi els requisitis necessaris per poder fer un thumbnail
    /// Retorna true si és un dataset vàlid, false altrament
    bool isSuitableForThumbnailCreation(DICOMTagReader *reader) const;

    /// Converteix la DicomImage a una QImage en format PGM/PPM
    /// depenent si la imatge és monocrom o de color.
    QImage convertToQImage(DicomImage *dicomImage);
};

}

#endif
