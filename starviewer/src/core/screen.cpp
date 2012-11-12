#include "screen.h"

#include <QString>

namespace udg {

const int Screen::NullScreenID = -1;

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
