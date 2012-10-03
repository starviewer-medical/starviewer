#include "imageoverlay.h"

#include "logging.h"
#include "drawerbitmap.h"
#include "imageoverlayregionfinder.h"
#include "mathtools.h"

#include <QRect>
#include <QRegExp>
#include <QStringList>

#include <gdcmOverlay.h>

namespace udg {

ImageOverlay::ImageOverlay()
{
    m_rows = 0;
    m_columns = 0;
    setOrigin(1, 1);
}

ImageOverlay::~ImageOverlay()
{
}

void ImageOverlay::setRows(int rows)
{
    m_rows = rows;
}

void ImageOverlay::setColumns(int columns)
{
    m_columns = columns;
}

int ImageOverlay::getRows() const
{
    return m_rows;
}

int ImageOverlay::getColumns() const
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

void ImageOverlay::deleteDataArray(unsigned char dataArray[])
{
    delete[] dataArray;
}

void ImageOverlay::setData(unsigned char *data)
{
    m_data = QSharedPointer<unsigned char>(data, deleteDataArray);
}

unsigned char* ImageOverlay::getData() const
{
    return m_data.data();
}

bool ImageOverlay::isValid() const
{
    return m_rows > 0 && m_columns > 0 && m_data;
}

ImageOverlay ImageOverlay::createSubOverlay(const QRect &region) const
{
    ImageOverlay subOverlay;

    if (this->isValid() && region.left() >= 0 && region.right() < m_columns && region.top() >= 0 && region.bottom() < m_rows)
    {
        subOverlay.setRows(region.height());
        subOverlay.setColumns(region.width());
        subOverlay.setOrigin(getXOrigin() + region.x(), getYOrigin() + region.y());
        subOverlay.setData(copyDataForSubOverlay(region));
    }

    return subOverlay;
}

QList<ImageOverlay> ImageOverlay::split() const
{
    ImageOverlayRegionFinder regionFinder(*this);
    regionFinder.findRegions(true);
    const QList<QRect> &regions = regionFinder.regions();

    QList<ImageOverlay> subOverlays;

    foreach (const QRect &region, regions)
    {
        subOverlays << createSubOverlay(region);
    }

    return subOverlays;
}

bool ImageOverlay::operator ==(const udg::ImageOverlay &overlay) const
{
    bool equal = this->m_rows == overlay.m_rows && this->m_columns == overlay.m_columns
              && this->m_origin[0] == overlay.m_origin[0] && this->m_origin[1] == overlay.m_origin[1]
              && ((this->m_data && overlay.m_data) || (!this->m_data && !overlay.m_data));

    if (equal && this->m_data)
    {
        equal = memcmp(this->m_data.data(), overlay.m_data.data(), this->m_rows * this->m_columns * sizeof(unsigned char)) == 0;
    }

    return equal;
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
            // #1903: degut a la implementació de GetUnpackBuffer, la mida del buffer ha de ser múltiple de 8
            int bufferSize = MathTools::roundUpToMultipleOfNumber(imageOverlay.getRows() * imageOverlay.getColumns(), 8);
            unsigned char *buffer = new unsigned char[bufferSize];
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

ImageOverlay ImageOverlay::mergeOverlays(const QList<ImageOverlay> &overlaysList, bool &ok)
{
    // Fem tria dels overlays que es puguin considerar vàlids
    QList<ImageOverlay> validOverlaysList;
    foreach (const ImageOverlay &overlay, overlaysList)
    {
        if (overlay.isValid())
        {
            validOverlaysList << overlay;
        }
    }

    if (validOverlaysList.isEmpty())
    {
        ok = true;
        return ImageOverlay();
    }

    int numberOfValidOverlays = validOverlaysList.count();

    if (numberOfValidOverlays == 1)
    {
        ok = true;
        return validOverlaysList.at(0);
    }
    
    // Tenim 2 o més overlays, per tant caldrà fer fusió
    
    // Primer calculem l'origen
    int outOriginX = 1;
    int outOriginY = 1;
    for (int i = 0; i < numberOfValidOverlays; ++i)
    {
        if (validOverlaysList.at(i).getXOrigin() < outOriginX)
        {
            outOriginX = validOverlaysList.at(i).getXOrigin();
        }

        if (validOverlaysList.at(i).getYOrigin() < outOriginY)
        {
            outOriginY = validOverlaysList.at(i).getYOrigin();
        }
    }

    // Ara calculem files i columnes
    int outColumns = 0;
    int outRows = 0;
    for (int i = 0; i < numberOfValidOverlays; ++i)
    {
        if (validOverlaysList.at(i).getColumns() + validOverlaysList.at(i).getXOrigin() - outOriginX > outColumns)
        {
            outColumns = validOverlaysList.at(i).getColumns() + validOverlaysList.at(i).getXOrigin() - outOriginX;
        }

        if (validOverlaysList.at(i).getRows() + validOverlaysList.at(i).getYOrigin() - outOriginY > outRows)
        {
            outRows = validOverlaysList.at(i).getRows() + validOverlaysList.at(i).getYOrigin() - outOriginY;
        }
    }
    
    // Ara creem el nou buffer únic i en fusionem les dades dels diferents overlays existents
    unsigned char *data = 0;
    try
    {
        data = new unsigned char[outColumns * outRows];
    }
    catch (std::bad_alloc)
    {
        ERROR_LOG(QString("No hi ha memòria suficient per crear el buffer per l'overlay fusionat [%1*%2] = %3 bytes")
            .arg(outRows).arg(outColumns).arg((unsigned long)outRows * outColumns));
        DEBUG_LOG(QString("No hi ha memòria suficient per crear el buffer per l'overlay fusionat [%1*%2] = %3 bytes")
            .arg(outRows).arg(outColumns).arg((unsigned long)outRows * outColumns));

        ok = false;
        return ImageOverlay();
    }

    // Hem pogut allotjar les dades, procedim a omplir-les amb les dades dels overlays
    memset(data, 0, sizeof(unsigned char) * outColumns * outRows);
        
    int subDataIndex;
    int subDataXIndex;
    int subDataYIndex;
    int outDataIndex;
    unsigned char outValue = 0;
    for (int outYIndex = 0; outYIndex < outRows; ++outYIndex)
    {
        for (int outXIndex = 0; outXIndex < outColumns; ++outXIndex)
        {
            outValue = 0;
            
            foreach (const ImageOverlay &overlay, validOverlaysList)
            {
                subDataXIndex = outOriginX - overlay.getXOrigin() + outXIndex;
                subDataYIndex = outOriginY - overlay.getYOrigin() + outYIndex;

                if (subDataXIndex >= 0 && subDataYIndex >=0 && subDataXIndex < overlay.getColumns() && subDataYIndex < overlay.getRows())
                {
                    subDataIndex = subDataXIndex + subDataYIndex * overlay.getColumns();
                    outValue |= overlay.getData()[subDataIndex];
                }
            }

            outDataIndex = outXIndex + outYIndex * outColumns;
            data[outDataIndex] = outValue;
        }
    }
    
    ImageOverlay imageOverlay;
    imageOverlay.setRows(outRows);
    imageOverlay.setColumns(outColumns);
    imageOverlay.setOrigin(outOriginX, outOriginY);
    imageOverlay.setData(data);

    ok = true;
    return imageOverlay;
}

DrawerBitmap* ImageOverlay::getAsDrawerBitmap(double origin[3], double spacing[3]) const
{
    DrawerBitmap *drawerBitmap = new DrawerBitmap;
    
    drawerBitmap->setSpacing(spacing);

    double bitmapOrigin[3];
    bitmapOrigin[0] = origin[0] + getXOrigin() * spacing[0];
    bitmapOrigin[1] = origin[1] + getYOrigin() * spacing[1];
    bitmapOrigin[2] = origin[2];
    drawerBitmap->setOrigin(bitmapOrigin);
    
    drawerBitmap->setData(getColumns(), getRows(), getData());
    
    return drawerBitmap;
}

unsigned char* ImageOverlay::copyDataForSubOverlay(const QRect &region) const
{
    unsigned char *regionData = new unsigned char[region.width() * region.height()];
    const unsigned char *overlayData = m_data.data();

    for (int i = 0; i < region.height(); i++)
    {
        int overlayRowOffset = (i + region.top()) * m_columns;
        int regionRowOffset = i * region.width();
        memcpy(regionData + regionRowOffset, overlayData + overlayRowOffset + region.left(), region.width() * sizeof(unsigned char));
    }

    return regionData;
}

}
