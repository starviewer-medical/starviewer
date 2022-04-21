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

#ifndef UDGIMAGEOVERLAYREADER_H
#define UDGIMAGEOVERLAYREADER_H

#include <QList>

namespace udg {

class ImageOverlay;

/**
    Classe per llegir overlays a través d'un arxiu. Per llegir els overlays caldrà assignar primer el nom de l'arxiu
    del que volem llegir els overlays i després fer-ne la lectura. Un cop feta la lectura podrem obtenir els overlays amb getOverlays()
 */
class ImageOverlayReader {
public:
    ImageOverlayReader();

    /// Nom del fitxer a partir del que hem de llegir l'overlay
    void setFilename(const QString &filename);
    /// Sets the number of overlays in the image.
    void setNumberOfOverlays(int numberOfOverlays);
    /// Sets the frame number of the image. Needed to get the proper overlay in multiframe overlays.
    void setFrameNumber(int frameNumber);

    /// Un cop donat un nom d'arxiu, en llegeix els overlays. Retorna fals en cas d'error, cert altrament
    bool read();

    /// Retorna la llista d'overlays llegits
    QList<ImageOverlay> getOverlays() const;

private:
    /// Nom de l'arxiu del que hem de llegir els overlays
    QString m_filename;
    /// Number of overlays in the image.
    int m_numberOfOverlays;
    /// Frame number of the image.
    int m_frameNumber;

    /// Llista d'overlays llegits
    QList<ImageOverlay> m_overlaysList;
};

}

#endif
