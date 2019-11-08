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

#ifndef UDG_VOLUMEFILLERSTEP_H
#define UDG_VOLUMEFILLERSTEP_H

#include "patientfillerstep.h"

#include "photometricinterpretation.h"
#include "pixelspacing2d.h"

#include <QHash>

namespace udg {

class Image;

/**
 * @brief The VolumeFillerStep class has the responsibility of assigning images to volumes.
 *
 * Additionally, it creates the volume thumbnails.
 */
class VolumeFillerStep : public PatientFillerStep
{
public:
    VolumeFillerStep(bool dontCreateThumbnails = false);

    virtual bool fillIndividually() override;

private:
    /// Donat un dicomReader guardem a la cache el corresponent thumbnail.
    /// La intenció d'aquest mètode és estalviar temps en la càrrega de thumbnails per arxius
    /// multiframe i enhanced ja que actualment és molt costós perquè hem de carregar tot el volum
    /// a memòria i aquí podem aprofitar que el dataset està a memòria evitant la càrrega posterior
    /// Tot i així es pot fer servir en altres casos que es cregui necessari avançar la creació del thumbnail
    void saveThumbnail(const Image *image);

private:
    /**
     * @brief The ImageProperties struct holds a set of distinct image properties.
     */
    struct ImageProperties
    {
        ImageProperties() = default;    // required by QMap
        ImageProperties(const Image *image);
        bool operator==(const ImageProperties &that) const;
        bool operator!=(const ImageProperties &that) const;

        bool multiframe;
        int rows;
        int columns;
        PhotometricInterpretation photometricInterpretation;
        PixelSpacing2D pixelSpacing;
    };

    /// ImageProperties for each volume number in each series.
    QHash<Series*, QMap<int, ImageProperties>> m_imagesProperties;
    /// If true, thumbnails won't be created.
    bool m_dontCreateThumbnails;

};

} // namespace udg

#endif // UDG_VOLUMEFILLERSTEP_H
