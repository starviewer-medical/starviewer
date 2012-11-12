#include "screen.h"

#include <QString>

namespace udg {

const int Screen::NullScreenID = -1;
const int Screen::MaximumDistanceInBetween = 5;

Screen::Screen()
{
    initializeValues();
}

Screen::Screen(const QRect &geometry, const QRect &availableGeometry)
{
    initializeValues();
    setGeometry(geometry);
    setAvailableGeometry(availableGeometry);
}

Screen::~Screen()
{
}

void Screen::setGeometry(const QRect &geometry)
{
    m_geometry = geometry;
}

QRect Screen::getGeometry() const
{
    return m_geometry;
}

void Screen::setAvailableGeometry(const QRect &geometry)
{
    m_availableGeometry = geometry;
}

QRect Screen::getAvailableGeometry() const
{
    return m_availableGeometry;
}

void Screen::setAsPrimary(bool isPrimary)
{
    m_isPrimary = isPrimary;
}

bool Screen::isPrimary() const
{
    return m_isPrimary;
}

void Screen::setID(int ID)
{
    m_ID = ID;
}

int Screen::getID() const
{
    return m_ID;
}

QString Screen::toString() const
{
    QString string;

    string = QString("Is Primary: %1\n").arg(m_isPrimary);
    string += QString("ID: %1\n").arg(m_ID);
    string += QString("Geometry: %1, %2, %3, %4\n").arg(m_geometry.x()).arg(m_geometry.y()).arg(m_geometry.width()).arg(m_geometry.height());
    string += QString("Available Geometry: %1, %2, %3, %4").arg(m_availableGeometry.x()).arg(m_availableGeometry.y()).arg(m_availableGeometry.width()).arg(m_availableGeometry.height());
    
    return string;
}

bool Screen::isHigher(const Screen &screen)
{
    if (m_geometry.top() < screen.getGeometry().top())
    {
        return true;
    }
    return false;
}

bool Screen::isLower(const Screen &screen)
{
    if (m_geometry.top() > screen.getGeometry().top())
    {
        return true;
    }
    return false;
}

bool Screen::isMoreToTheLeft(const Screen &screen)
{
    if (m_geometry.left() < screen.getGeometry().left())
    {
        return true;
    }
    return false;
}

bool Screen::isMoreToTheRight(const Screen &screen)
{
    if (m_geometry.right() > screen.getGeometry().right())
    {
        return true;
    }
    return false;
}

bool Screen::isOver(const Screen &screen) const
{
    if (m_geometry.bottom() <= screen.getGeometry().top())
    {
        return true;
    }
    return false;
}

bool Screen::isUnder(const Screen &screen) const
{
    if (m_geometry.top() >= screen.getGeometry().bottom())
    {
        return true;
    }
    return false;
}

bool Screen::isOnLeft(const Screen &screen) const
{
    if (m_geometry.right() <= screen.getGeometry().left())
    {
        return true;
    }
    return false;
}

bool Screen::isOnRight(const Screen &screen) const
{
    if (m_geometry.left() >= screen.getGeometry().right())
    {
        return true;
    }
    return false;
}

bool Screen::isTop(const Screen &screen) const
{
    // Esta posat a sobre
    if (abs(m_geometry.bottom() - screen.getGeometry().top()) < MaximumDistanceInBetween)
    {
        // Te la mateixa alçada
        int leftPart = abs(m_geometry.left() - screen.getGeometry().left());
        int rightPart = abs(m_geometry.right() - screen.getGeometry().right());
        if (leftPart + rightPart < MaximumDistanceInBetween)
        {
            return true;
        }
    }

    return false;
}

bool Screen::isBottom(const Screen &screen) const
{
    // Esta posat a sota
    if (abs(m_geometry.top() - screen.getGeometry().bottom()) < MaximumDistanceInBetween)
    {
        // Te la mateixa alçada
        int leftPart = abs(m_geometry.left() - screen.getGeometry().left());
        int rightPart = abs(m_geometry.right() - screen.getGeometry().right());
        if (leftPart + rightPart < MaximumDistanceInBetween)
        {
            return true;
        }
    }

    return false;
}

bool Screen::isLeft(const Screen &screen) const
{
    // Esta posat a l'esquerra
    if (abs(m_geometry.right() - screen.getGeometry().left()) < MaximumDistanceInBetween)
    {
        // Te la mateixa alçada
        int topPart = abs(m_geometry.top() - screen.getGeometry().top());
        int bottomPart = abs(m_geometry.bottom() - screen.getGeometry().bottom());
        if (topPart + bottomPart < MaximumDistanceInBetween)
        {
            return true;
        }
    }
    return false;
}

bool Screen::isRight(const Screen &screen) const
{
    // Esta posat a l'esquerra
    if (abs(m_geometry.left() - screen.getGeometry().right()) < MaximumDistanceInBetween)
    {
        // Te la mateixa alçada
        int topPart = abs(m_geometry.top() - screen.getGeometry().top());
        int bottomPart = abs(m_geometry.bottom() - screen.getGeometry().bottom());
        if (topPart + bottomPart < MaximumDistanceInBetween)
        {
            return true;
        }
    }
    return false;
}

bool Screen::isTopLeft(const Screen &screen) const
{
    QPoint distancePoint = m_geometry.bottomRight() - screen.getGeometry().topLeft();
    
    if (distancePoint.manhattanLength() < MaximumDistanceInBetween)
    {
        return true;
    }
    return false;
}

bool Screen::isTopRight(const Screen &screen) const
{
    QPoint distancePoint = m_geometry.bottomLeft() - screen.getGeometry().topRight();
    
    if (distancePoint.manhattanLength() < MaximumDistanceInBetween)
    {
        return true;
    }
    return false;
}

bool Screen::isBottomLeft(const Screen &screen) const
{
    QPoint distancePoint = m_geometry.topRight() - screen.getGeometry().bottomLeft();
    
    if (distancePoint.manhattanLength() < MaximumDistanceInBetween)
    {
        return true;
    }
    return false;
}

bool Screen::isBottomRight(const Screen &screen) const
{
    QPoint distancePoint = m_geometry.topLeft() - screen.getGeometry().bottomRight();
    
    if (distancePoint.manhattanLength() < MaximumDistanceInBetween)
    {
        return true;
    }
    return false;
}

bool Screen::operator==(const Screen &screen) const
{
    return m_isPrimary == screen.m_isPrimary
        && m_ID == screen.m_ID
        && m_geometry == screen.m_geometry
        && m_availableGeometry == screen.m_availableGeometry;
}

void Screen::initializeValues()
{
    m_isPrimary = false;
    m_ID = NullScreenID;
}

} // End namespace udg
