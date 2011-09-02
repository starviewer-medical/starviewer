#include "imageoverlay.h"

#include <QRegExp>
#include <QStringList>

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

}
