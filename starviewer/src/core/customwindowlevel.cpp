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

void CustomWindowLevel::setWindow(double window)
{
    m_window = window;
}

void CustomWindowLevel::setLevel(double level)
{
    m_level = level;
}

QString CustomWindowLevel::getName() const
{
    return m_name;
}

double CustomWindowLevel::getWindow() const
{
    return m_window;
}

double CustomWindowLevel::getLevel() const
{
    return m_level;
}

}
#endif
