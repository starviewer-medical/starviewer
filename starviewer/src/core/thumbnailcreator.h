/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGTHUMBNAILCREATOR_H
#define UDGTHUMBNAILCREATOR_H

class QImage;
class QPixmap;
class QString;
class DicomImage;

namespace udg {

class Series;
class Image;
class DICOMTagReader;

class ThumbnailCreator {
public:
    /// Crea un thumbnail a partir de les imatges de la sèrie
    QImage getThumbnail(const Series *series, int resolution = 96);

    /// Crea el thumbnail de la imatge passada per paràmetre
    QImage getThumbnail(const Image *image, int resolution = 96);

    /// Obté el thumbnail a partir del DICOMTagReader
    QImage getThumbnail(const DICOMTagReader *reader, int resolution = 96);

    /// Crea un thumbnail buit personalitzat amb el text que li donem
    static QImage makeEmptyThumbnailWithCustomText(const QString &text, int resolution = 96);

private:
    /// Crea el thumbnail d'un objecte dicom que sigui una imatge
    QImage createImageThumbnail(const QString &imageFileName, int resolution);

    /// Creates a thumbnail from an icon file to the specified resolution
    QImage createIconThumbnail(const QString &iconFileName, int resolution);

    /// Crea el thumbnail a partir d'un DICOMTagReader
    QImage createThumbnail(const DICOMTagReader *reader, int resolution);

    /// Crea el thumbnail a partir d'una DicomImage
    QImage createThumbnail(DicomImage *dicomImage, int resolution);

    /// Comprova que el dataset compleixi els requisitis necessaris per poder fer un thumbnail
    /// Retorna true si és un dataset vàlid, false altrament
    bool isSuitableForThumbnailCreation(const DICOMTagReader *reader) const;

    /// Converteix la DicomImage a una QPixmap
    QPixmap convertToQPixmap(DicomImage *dicomImage);
};

}

#endif
