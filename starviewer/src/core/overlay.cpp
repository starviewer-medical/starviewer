#include "overlay.h"

#include <QRegExp>
#include <QStringList>

namespace udg {

const QChar Overlay::GraphicsType('G');
const QChar Overlay::ROIType('R');

Overlay::Overlay()
{
    m_rows = 0;
    m_columns = 0;
    setDICOMFormattedOrigin("1\\1");
}

Overlay::~Overlay()
{
}

void Overlay::setRows(unsigned int rows)
{
    m_rows = rows;
}

void Overlay::setColumns(unsigned int columns)
{
    m_columns = columns;
}

unsigned int Overlay::getRows() const
{
    return m_rows;
}

unsigned int Overlay::getColumns() const
{
    return m_columns;
}

void Overlay::setType(const QChar &overlayType)
{
    if (overlayType != GraphicsType && overlayType != ROIType)
    {
        return;
    }
    else
    {
        m_type = overlayType;
    }
}

QChar Overlay::getType() const
{
    return m_type;
}

void Overlay::setDICOMFormattedOrigin(const QString &origin)
{
    QRegExp originExpression("^-?\\d+\\\\-?\\d+$");
    if (originExpression.exactMatch(origin))
    {
        m_origin = origin;
    }
}

int Overlay::getXOrigin() const
{
    return m_origin.split("\\").at(0).toInt();
}

int Overlay::getYOrigin() const
{
    return m_origin.split("\\").at(1).toInt();
}

}
