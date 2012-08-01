#include "windowlevel.h"
#include <QString>

namespace udg {

WindowLevel::WindowLevel()
{
    m_width = 0.0;
    m_level = 0.0;
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

void WindowLevel::setLevel(double level)
{
    m_level = level;
}

QString WindowLevel::getName() const
{
    return m_name;
}

double WindowLevel::getWidth() const
{
    return m_width;
}

double WindowLevel::getLevel() const
{
    return m_level;
}

bool WindowLevel::isValid() const
{
    if (m_width == 0.0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

}
