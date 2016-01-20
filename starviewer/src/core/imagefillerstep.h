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

#ifndef UDGIMAGEFILLERSTEP_H
#define UDGIMAGEFILLERSTEP_H

#include "patientfillerstep.h"
#include "dicomtag.h"

class QVector3D;

namespace udg {

class Patient;
class Series;
class Image;
class DICOMTagReader;
class DICOMSequenceItem;
class ImageOrientation;

/**
    Mòdul que s'encarrega d'omplir la informació general d'objectes DICOM que són imatges. Un dels seus requisits és que es tingui l'etiqueta de
    DICOMClassified i que la Series a tractar sigui d'imatges
  */
class ImageFillerStep : public PatientFillerStep {
public:
    ImageFillerStep();
    ~ImageFillerStep();

    bool fillIndividually();

    void postProcessing() {}

    QString name()
    {
        return "ImageFillerStep";
    }

private:
    /// Mètode per processar la informació específica de pacient,series i imatge
    bool processImage(Image *image, DICOMTagReader *dicomReader);

    /// Mètode encarregat de processar el fitxer DICOM per extreure'n el conjunt de les imatges
    /// que el conformen, omplint la informació necessària
    QList<Image*> processDICOMFile(DICOMTagReader *dicomReader);

    /// Mètode específic per processar els arxius que siguin de tipus Enhanced
    QList<Image*> processEnhancedDICOMFile(DICOMTagReader *dicomReader);

    /// Mètode encarregat de processar el fitxer DICOM per extreure'n el conjunt de les imatges
    /// que el conformen, omplint la informació necessària
    /// HACK Hitachi
    QList<Image*> processDICOMFileHitachi(DICOMTagReader *dicomReader);

    /// Mètode específic per processar els arxius que siguin de tipus Enhanced
    /// HACK Hitachi
    QList<Image*> processEnhancedDICOMFileHitachi(DICOMTagReader *dicomReader);

    /// Donat un dicomReader guardem a la cache el corresponent thumbnail.
    /// La intenció d'aquest mètode és estalviar temps en la càrrega de thumbnails per arxius
    /// multiframe i enhanced ja que actualment és molt costós perquè hem de carregar tot el volum
    /// a memòria i aquí podem aprofitar que el dataset està a memòria evitant la càrrega posterior
    /// Tot i així es pot fer servir en altres casos que es cregui necessari avançar la creació del thumbnail
    void saveThumbnail(DICOMTagReader *dicomReader);

    /// Omple la informació comú a totes les imatges.
    /// Image i dicomReader han de ser objectes vàlids.
    bool fillCommonImageInformation(Image *image, DICOMTagReader *dicomReader);

    /// Omple l'image donat amb la informació dels functional groups continguts en l'ítem proporcionat
    /// Aquest mètode està pensat per fer-se servir amb els ítems obtinguts
    /// tant amb la Shared Functional Groups Sequence com amb la Per-Frame Functional Groups Sequence
    void fillFunctionalGroupsInformation(Image *image, DICOMSequenceItem *frameItem);

    /// Retorna quants overlays hi ha en el dataset proporcionat
    unsigned short getNumberOfOverlays(DICOMTagReader *dicomReader);
    
    /// Afegeix la informació de shutters a la imatge, si n'hi ha
    void fillDisplayShutterInformation(Image *image, DICOMTagReader *dicomReader);
    void fillDisplayShutterInformation(Image *image, DICOMSequenceItem *displayShutterItems);
    
    /// Calcula el pixel spacing i se l'assigna a l'image donada en cas de que aquest es pugui calcular
    /// @param image Image a la que li assignarem el pixel spacing
    /// @param dicomReader Reader de DICOM que conté la font de dades de la Image associada
    void computePixelSpacing(Image *image, DICOMTagReader *dicomReader);

    /// Checks and sets the Estimated Radiographic Magnification Factor tag for the corresponding modalities
    void checkAndSetEstimatedRadiographicMagnificationFactor(Image *image, DICOMTagReader *dicomReader);

    /// Ens diu si les imatges són de mides diferents
    bool areOfDifferentSize(Image *firstImage, Image *secondImage);

    /// Ens diu si les imatges tenen photometric interpretations diferents
    bool areOfDifferentPhotometricInterpretation(Image *firstImage, Image *secondImage);

    /// Ens diu si les imatges tenen pixel spacing diferents
    bool areOfDifferentPixelSpacing(Image *firstImage, Image *secondImage);

    /// Ens diu si el SOP Class UID es correspon amb el d'una imatge enhanced
    bool isEnhancedImageSOPClass(const QString &sopClassUID);

    /// Validates the spacing string and sets it to the given image if it's well formatted.
    /// If no pixel spacing tag is specified, Pixel Spacing will be used by default
    /// PixelSpacing and ImagerPixelSpacing tags are the only tags supported currently
    /// Returns true on success, false otherwise
    bool validateAndSetSpacingAttribute(Image *image, const QString &spacing, const DICOMTag &tag = DICOMTag(0x0028, 0x0030));
};

}

#endif
