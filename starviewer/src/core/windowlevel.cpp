#include "windowlevel.h"
#include <QString>

namespace udg {

WindowLevel::WindowLevel()
{
    m_width = 0.0;
    m_center = 0.0;
}

WindowLevel::WindowLevel(double width, double center, const QString &name)
{
    m_width = width;
    m_center = center;
    m_name = name;
}

WindowLevel::~WindowLevel()
{
}

void WindowLevel::setName(const QString &name)
{
    m_name = name;
}

void WindowLevel::setWidth(double width)
{
    m_width = width;
}

void WindowLevel::setCenter(double center)
{
    m_center = center;
}

QString WindowLevel::getName() const
{
    return m_name;
}

double WindowLevel::getWidth() const
{
    return m_width;
}

double WindowLevel::getCenter() const
{
    return m_center;
}

bool WindowLevel::isValid() const
{
    if (std::abs(m_width) < 1.0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool WindowLevel::valuesAreEqual(const WindowLevel &windowLevel) const
{
    if (m_width == windowLevel.getWidth() && m_center == windowLevel.getCenter())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool WindowLevel::operator==(const WindowLevel &windowLevelToCompare) const
{
    if (m_width == windowLevelToCompare.getWidth() && m_center == windowLevelToCompare.getCenter() && m_name == windowLevelToCompare.getName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

}
