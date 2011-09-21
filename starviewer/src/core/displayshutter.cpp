#include "displayshutter.h"

#include "mathtools.h"
#include <cmath>

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

void DisplayShutter::setShutterValue(unsigned short int value)
{
    m_shutterValue = value;
}

unsigned short int DisplayShutter::getShutterValue() const
{
    return m_shutterValue;
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

} // End namespace udg
