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

#include "imageoverlayreader.h"

#include "logging.h"

#include <gdcmImageReader.h>
#include <gdcmOverlay.h>

namespace udg {

ImageOverlayReader::ImageOverlayReader()
{
}

ImageOverlayReader::~ImageOverlayReader()
{
}

void ImageOverlayReader::setFilename(const QString &filename)
{
    m_filename = filename;
}
  
bool ImageOverlayReader::read()
{
    m_overlaysList.clear();

    gdcm::Image image = getGDCMImageFromFile(m_filename);
    
    for (size_t overlayIndex = 0; overlayIndex < image.GetNumberOfOverlays(); ++overlayIndex)
    {
        m_overlaysList << ImageOverlay::fromGDCMOverlay(image.GetOverlay(overlayIndex));
    }

    return true;
}

QList<ImageOverlay> ImageOverlayReader::getOverlays() const
{
    return m_overlaysList;
}

gdcm::Image ImageOverlayReader::getGDCMImageFromFile(const QString &filename)
{
    gdcm::ImageReader imageReader;
    imageReader.SetFileName(qPrintable(filename));
    if (!imageReader.Read())
    {
        ERROR_LOG("Ha fallat la lectura del fitxer: " + filename + " [ImageOverlayReader]");
        DEBUG_LOG("Ha fallat la lectura del fitxer: " + filename);
        return gdcm::Image();
    }
    
    return imageReader.GetImage();
}

}
