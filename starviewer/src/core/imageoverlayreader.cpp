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

    gdcm::Image *image = getGDCMImageFromFile(m_filename);
    if (!image)
    {
        return 0;
    }
    
    for (size_t overlayIndex = 0; overlayIndex < image->GetNumberOfOverlays(); ++overlayIndex)
    {
        m_overlaysList << ImageOverlay::fromGDCMOverlay(image->GetOverlay(overlayIndex));
    }
    
    return true;
}

QList<ImageOverlay> ImageOverlayReader::getOverlays() const
{
    return m_overlaysList;
}

gdcm::Image* ImageOverlayReader::getGDCMImageFromFile(const QString &filename)
{
    gdcm::ImageReader imageReader;
    imageReader.SetFileName(qPrintable(filename));
    if (!imageReader.Read())
    {
        ERROR_LOG("Ha fallat la lectura del fitxer: " + filename + " [ImageOverlayReader]");
        DEBUG_LOG("Ha fallat la lectura del fitxer: " + filename);
        return 0;
    }
    
    return &imageReader.GetImage();
}

}
