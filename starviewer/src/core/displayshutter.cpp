#include "displayshutter.h"

#include "mathtools.h"
#include <cmath>

#include <QColor>
#include <QRegExp>

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

        default:
            return "";
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

bool DisplayShutter::setPoints(const QString &pointsString)
{
    if (m_shape == UndefinedShape)
    {
        if (shapeMatchesPointsStringFormat(RectangularShape, pointsString))
        {
            m_shape = RectangularShape;
        }
        else if (shapeMatchesPointsStringFormat(CircularShape, pointsString))
        {
            m_shape = CircularShape;
        }
        else if (shapeMatchesPointsStringFormat(PolygonalShape, pointsString))
        {
            m_shape = PolygonalShape;
        }
        else
        {
            return false;
        }
    }
    else if (!shapeMatchesPointsStringFormat(m_shape, pointsString))
    {
        return false;
    }
    
    QStringList pointsList = pointsString.split(";", QString::SkipEmptyParts);
    
    switch (m_shape)
    {
        case RectangularShape:
            {
                if (pointsList.count() != 2)
                {
                    return false;
                }
                
                QStringList topLeft = pointsList.at(0).split(",");
                QStringList bottomRight = pointsList.at(1).split(",");
                return setPoints(QPoint(topLeft.at(0).toInt(), topLeft.at(1).toInt()), QPoint(bottomRight.at(0).toInt(), bottomRight.at(1).toInt()));
            }
            break;

        case CircularShape:
            {
                if (pointsList.count() != 2)
                {
                    return false;
                }
                
                QStringList centre = pointsList.at(0).split(",");
                return setPoints(QPoint(centre.at(0).toInt(), centre.at(1).toInt()), pointsList.at(1).toInt());
            }
            break;

        case PolygonalShape:
            {
                QPolygon polygon;
                
                for (int i = 0; i < pointsList.count(); ++i)
                {
                    QStringList point = pointsList.at(i).split(",");
                    polygon << QPoint(point.at(0).toInt(), point.at(1).toInt());
                }
                
                return setPoints(polygon);
            }
            break;
    }

    return false;
}

QPolygon DisplayShutter::getAsQPolygon() const
{
    return m_shutterPolygon;
}

QString DisplayShutter::getPointsAsString() const
{
    QString pointsString;
    
    switch (m_shape)
    {
        case DisplayShutter::UndefinedShape:
            break;

        case DisplayShutter::RectangularShape:
            {
                QPoint topLeft = m_shutterPolygon.at(0);
                QPoint bottomRight = m_shutterPolygon.at(2);
                pointsString = QString("%1,%2;%3,%4").arg(topLeft.x()).arg(topLeft.y()).arg(bottomRight.x()).arg(bottomRight.y());
            }
            break;

        case DisplayShutter::CircularShape:
            {
                QPoint firstPoint = m_shutterPolygon.first();
                QPoint midPoint = m_shutterPolygon.at(m_shutterPolygon.count() / 2);

                int radius = abs(firstPoint.x() - midPoint.x()) / 2;
                QPoint centre;
                centre.setX(firstPoint.x() - radius);
                centre.setY(firstPoint.y());
                pointsString = QString("%1,%2;%3").arg(centre.x()).arg(centre.y()).arg(radius);
            }
            break;

        case DisplayShutter::PolygonalShape:
            foreach(const QPoint &point, m_shutterPolygon)
            {
                pointsString += QString("%1,%2;").arg(point.x()).arg(point.y());
            }
            // Eliminem el ; final per complir amb el format
            pointsString.chop(1);
            break;
    }

    return pointsString;
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

bool DisplayShutter::shapeMatchesPointsStringFormat(ShapeType shape, const QString &pointsString)
{
    QRegExp expression;
    const QString IntegerPatternString("-?\\d+");
    const QString PointPatternString(IntegerPatternString + "," + IntegerPatternString);
    switch (shape)
    {
        case RectangularShape:
            // 2 punts separats per ;
            expression.setPattern(PointPatternString + ";" + PointPatternString);
            break;
            
        case CircularShape:
            // Centre;radi
            expression.setPattern(PointPatternString + ";" + IntegerPatternString);
            break;

        case PolygonalShape:
            // Com a mínim 3 punts separats per ;
            expression.setPattern("(" + PointPatternString + ";){2,}" + PointPatternString);
            break;

        case UndefinedShape:
            // Si no hi ha forma definida, no hi ha cap patró vàlid
            return false;
            break;
    }

    return expression.exactMatch(pointsString);
}

} // End namespace udg
