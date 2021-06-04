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

namespace udg {

class DICOMSequenceItem;
class DICOMTagReader;
class Image;

/**
 * @brief The ImageFillerStep class creates and fills Image objects from DICOM files.
 */
class ImageFillerStep : public PatientFillerStep {

public:
    ImageFillerStep();
    virtual ~ImageFillerStep();

    virtual bool fillIndividually() override;

private:
    /// Mètode per processar la informació específica de pacient,series i imatge
    void processImage(Image *image, const DICOMTagReader *dicomReader);

    /// Mètode encarregat de processar el fitxer DICOM per extreure'n el conjunt de les imatges
    /// que el conformen, omplint la informació necessària
    QList<Image*> processDICOMFile(const DICOMTagReader *dicomReader);

    /// Mètode específic per processar els arxius que siguin de tipus Enhanced
    QList<Image*> processEnhancedDICOMFile(const DICOMTagReader *dicomReader);

    /// Omple la informació comú a totes les imatges.
    /// Image i dicomReader han de ser objectes vàlids.
    void fillCommonImageInformation(Image *image, const DICOMTagReader *dicomReader);

    /// Omple l'image donat amb la informació dels functional groups continguts en l'ítem proporcionat
    /// Aquest mètode està pensat per fer-se servir amb els ítems obtinguts
    /// tant amb la Shared Functional Groups Sequence com amb la Per-Frame Functional Groups Sequence
    void fillFunctionalGroupsInformation(Image *image, DICOMSequenceItem *frameItem);

    /// Retorna quants overlays hi ha en el dataset proporcionat
    unsigned short getNumberOfOverlays(const DICOMTagReader *dicomReader);
    
    /// Calcula el pixel spacing i se l'assigna a l'image donada en cas de que aquest es pugui calcular
    /// @param image Image a la que li assignarem el pixel spacing
    /// @param dicomReader Reader de DICOM que conté la font de dades de la Image associada
    void computePixelSpacing(Image *image, const DICOMTagReader *dicomReader);

    /// Checks and sets the Estimated Radiographic Magnification Factor tag for the corresponding modalities
    void checkAndSetEstimatedRadiographicMagnificationFactor(Image *image, const DICOMTagReader *dicomReader);

    /// Ens diu si el SOP Class UID es correspon amb el d'una imatge enhanced
    bool isEnhancedImageSOPClass(const QString &sopClassUID);

    /// Validates the spacing string and sets it to the given image if it's well formatted.
    /// If no pixel spacing tag is specified, Pixel Spacing will be used by default
    /// PixelSpacing and ImagerPixelSpacing tags are the only tags supported currently
    /// Returns true on success, false otherwise
    void validateAndSetSpacingAttribute(Image *image, const QString &spacing, const DICOMTag &tag = DICOMTag(0x0028, 0x0030));

};

}

#endif
