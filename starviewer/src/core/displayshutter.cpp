#include "displayshutter.h"

#include "mathtools.h"
#include <cmath>

#include <QColor>

namespace udg {

DisplayShutter::DisplayShutter()
{
    m_shape = UndefinedShape;
    m_shutterValue = 0;
}

DisplayShutter::~DisplayShutter()
{
}

void DisplayShutter::setShape(ShapeType shape)
{
    m_shape = shape;
}

DisplayShutter::ShapeType DisplayShutter::getShape() const
{
    return m_shape;
}

QString DisplayShutter::getShapeAsDICOMString() const
{
    switch (m_shape)
    {
        case DisplayShutter::UndefinedShape:
            return "";

        case DisplayShutter::RectangularShape:
            return "RECTANGULAR";

        case DisplayShutter::CircularShape:
            return "CIRCULAR";

        case DisplayShutter::PolygonalShape:
            return "POLYGONAL";
    }
}

void DisplayShutter::setShutterValue(unsigned short int value)
{
    m_shutterValue = value;
}

unsigned short int DisplayShutter::getShutterValue() const
{
    return m_shutterValue;
}

QColor DisplayShutter::getShutterValueAsQColor() const
{
    // El rang de valors del shutter pot anar de 0 a 65535 (0xFFFF)
    // Cal escalar aquest valor al rang que accepta QColor en format HSV que va de 0 a 255 (0xFF)
    return QColor::fromHsv(0, 0, (m_shutterValue * 255) / 0xFFFF);
}

bool DisplayShutter::setPoints(const QPoint &centre, int radius)
{
    if (m_shape == UndefinedShape)
    {
        m_shape = CircularShape;
    }
    
    if (m_shape != CircularShape)
    {
        return false;
    }
    
    m_shutterPolygon.clear();
    const int PolygonCirclePoints = 50;
    double alpha;
    double sinusAlpha;
    double cosinusAlpha;
    for (double i = 0; i < 360.0; i+= 360.0 / PolygonCirclePoints)
    {
        alpha = MathTools::degreesToRadians(i);
        sinusAlpha = sin(alpha);
        cosinusAlpha = cos(alpha);

        QPoint polygonPoint;
        polygonPoint.setX(centre.x() + (radius * cosinusAlpha));
        polygonPoint.setY(centre.y() + (radius * sinusAlpha));

        m_shutterPolygon << polygonPoint;
    }
    
    return true;
}

bool DisplayShutter::setPoints(const QVector<QPoint> &vertices)
{
    if (m_shape == UndefinedShape)
    {
        m_shape = PolygonalShape;
    }
    
    if (m_shape != PolygonalShape)
    {
        return false;
    }
    
    m_shutterPolygon.clear();
    m_shutterPolygon = vertices;

    return true;
}

bool DisplayShutter::setPoints(const QPoint &topLeft, const QPoint &bottomRight)
{
    if (m_shape == UndefinedShape)
    {
        m_shape = RectangularShape;
    }
    
    if (m_shape != RectangularShape)
    {
        return false;
    }
    
    m_shutterPolygon.clear();
    m_shutterPolygon << topLeft; 
    m_shutterPolygon << QPoint(bottomRight.x(), topLeft.y());
    m_shutterPolygon << bottomRight;
    m_shutterPolygon << QPoint(topLeft.x(), bottomRight.y());

    return true;
}

QPolygon DisplayShutter::getAsQPolygon() const
{
    return m_shutterPolygon;
}

DisplayShutter DisplayShutter::intersection(const QList<DisplayShutter> &shuttersList)
{
    if (shuttersList.isEmpty())
    {
        return DisplayShutter();
    }

    if (shuttersList.count() == 1)
    {
        return shuttersList.at(0);
    }
    
    QPolygon intersectedPolygon;
    unsigned int shutterComposedColor = 0;
    
    for (int i = 0; i < shuttersList.count() - 1; ++i)
    {
        QPolygon currentPolygon;
        if (intersectedPolygon.isEmpty())
        {
            currentPolygon = shuttersList.at(i).m_shutterPolygon;
        }
        else
        {
            currentPolygon = intersectedPolygon;
        }
        
        QPolygon nextPolygon = shuttersList.at(i + 1).m_shutterPolygon;
        intersectedPolygon = currentPolygon.intersected(nextPolygon);

        shutterComposedColor += shuttersList.at(i).getShutterValue();
    }
    shutterComposedColor += shuttersList.last().getShutterValue();
    shutterComposedColor = shutterComposedColor / shuttersList.count();
    
    DisplayShutter intersectedShutter;
    if (!intersectedPolygon.isEmpty())
    {
        intersectedShutter.setShape(DisplayShutter::PolygonalShape);
        intersectedShutter.setPoints(intersectedPolygon);
        intersectedShutter.setShutterValue(shutterComposedColor);
    }

    return intersectedShutter;
}

} // End namespace udg
