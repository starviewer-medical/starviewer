#include "imageoverlay.h"

#include "logging.h"
#include "drawerbitmap.h"

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

DrawerBitmap* ImageOverlay::getAsDrawerBitmap(double origin[3], double spacing[3])
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

}
