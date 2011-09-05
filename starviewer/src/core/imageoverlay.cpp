#include "imageoverlay.h"

#include "logging.h"

#include <QRegExp>
#include <QStringList>

#include <gdcmOverlay.h>

namespace udg {

ImageOverlay::ImageOverlay()
{
    m_rows = 0;
    m_columns = 0;
    setOrigin(1, 1);
    m_data = 0;
}

ImageOverlay::~ImageOverlay()
{
}

void ImageOverlay::setRows(unsigned int rows)
{
    m_rows = rows;
}

void ImageOverlay::setColumns(unsigned int columns)
{
    m_columns = columns;
}

unsigned int ImageOverlay::getRows() const
{
    return m_rows;
}

unsigned int ImageOverlay::getColumns() const
{
    return m_columns;
}

void ImageOverlay::setOrigin(int x, int y)
{
    m_origin[0] = x;
    m_origin[1] = y;
}

int ImageOverlay::getXOrigin() const
{
    return m_origin[0];
}

int ImageOverlay::getYOrigin() const
{
    return m_origin[1];
}

void ImageOverlay::setData(unsigned char *data)
{
    m_data = data;
}

unsigned char* ImageOverlay::getData() const
{
    return m_data;
}

ImageOverlay ImageOverlay::fromGDCMOverlay(const gdcm::Overlay &gdcmOverlay)
{
    ImageOverlay imageOverlay;
    imageOverlay.setRows(gdcmOverlay.GetRows());
    imageOverlay.setColumns(gdcmOverlay.GetColumns());
    const signed short *origin = gdcmOverlay.GetOrigin();
    imageOverlay.setOrigin(static_cast<int>(origin[0]), static_cast<int>(origin[1]));
    
    if (imageOverlay.getColumns() == 0 || imageOverlay.getRows() == 0)
    {
        imageOverlay.setData(0);
    }
    else
    {
        try
        {
            unsigned char *buffer = new unsigned char[imageOverlay.getRows() * imageOverlay.getColumns()];
            gdcmOverlay.GetUnpackBuffer(buffer);
            imageOverlay.setData(buffer);
        }
        catch (std::bad_alloc)
        {
            imageOverlay.setData(0);
            
            ERROR_LOG(QString("No hi ha memòria suficient per carregar l'overlay [%1*%2] = %3 bytes")
                .arg(imageOverlay.getRows()).arg(imageOverlay.getColumns()).arg((unsigned long)imageOverlay.getRows() * imageOverlay.getColumns()));
            DEBUG_LOG(QString("No hi ha memòria suficient per carregar l'overlay [%1*%2] = %3 bytes")
                .arg(imageOverlay.getRows()).arg(imageOverlay.getColumns()).arg((unsigned long)imageOverlay.getRows() * imageOverlay.getColumns()));
        }
    }

    return imageOverlay;
}

}
