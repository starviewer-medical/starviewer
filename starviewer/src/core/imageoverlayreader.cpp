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
        const gdcm::Overlay &gdcmOverlay = image->GetOverlay(overlayIndex);
        
        ImageOverlay imageOverlay;
        imageOverlay.setRows(gdcmOverlay.GetRows());
        imageOverlay.setColumns(gdcmOverlay.GetColumns());
        const signed short *origin = gdcmOverlay.GetOrigin();
        imageOverlay.setOrigin(static_cast<int>(origin[0]), static_cast<int>(origin[1]));

        unsigned char *buffer = 0;
        try
        {
            buffer = new unsigned char[imageOverlay.getRows() * imageOverlay.getColumns()];
            gdcmOverlay.GetUnpackBuffer(buffer);
            imageOverlay.setData(buffer);
        }
        catch (std::bad_alloc)
        {
            ERROR_LOG(QString("No hi ha memòria suficient per carregar l'overlay [%1*%2] = %3 bytes")
                .arg(imageOverlay.getRows()).arg(imageOverlay.getColumns()).arg((unsigned long)imageOverlay.getRows() * imageOverlay.getColumns()));
            DEBUG_LOG(QString("No hi ha memòria suficient per carregar l'overlay [%1*%2] = %3 bytes")
                .arg(imageOverlay.getRows()).arg(imageOverlay.getColumns()).arg((unsigned long)imageOverlay.getRows() * imageOverlay.getColumns()));
        }
        m_overlaysList << imageOverlay;
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
