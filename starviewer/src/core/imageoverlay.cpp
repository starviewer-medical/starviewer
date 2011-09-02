#include "imageoverlay.h"

#include <QRegExp>
#include <QStringList>

namespace udg {

ImageOverlay::ImageOverlay()
{
    m_rows = 0;
    m_columns = 0;
    setDICOMFormattedOrigin("1\\1");
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

void ImageOverlay::setDICOMFormattedOrigin(const QString &origin)
{
    QRegExp originExpression("^-?\\d+\\\\-?\\d+$");
    if (originExpression.exactMatch(origin))
    {
        m_origin = origin;
    }
}

int ImageOverlay::getXOrigin() const
{
    return m_origin.split("\\").at(0).toInt();
}

int ImageOverlay::getYOrigin() const
{
    return m_origin.split("\\").at(1).toInt();
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
