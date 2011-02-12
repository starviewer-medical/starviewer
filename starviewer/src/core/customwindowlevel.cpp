#ifndef UDGCUSTOMWINDOWLEVEL_CPP
#define UDGCUSTOMWINDOWLEVEL_CPP

#include "customwindowlevel.h"
#include <QString>

namespace udg {

CustomWindowLevel::CustomWindowLevel()
{
}

CustomWindowLevel::~CustomWindowLevel()
{
}

void CustomWindowLevel::setName(const QString &name)
{
    m_name = name;
}

void CustomWindowLevel::setWidth(double width)
{
    m_width = width;
}

void CustomWindowLevel::setLevel(double level)
{
    m_level = level;
}

QString CustomWindowLevel::getName() const
{
    return m_name;
}

double CustomWindowLevel::getWidth() const
{
    return m_width;
}

double CustomWindowLevel::getLevel() const
{
    return m_level;
}

}
#endif
