#include "screenlayout.h"

#include <QDesktopWidget>
#include <QApplication>

namespace udg {

const int ScreenLayout::SamePositionThreshold = 5;

ScreenLayout::ScreenLayout()
{
}

ScreenLayout::~ScreenLayout()
{
}

bool ScreenLayout::addScreen(const Screen &screen)
{
    if (screen.getID() <= Screen::NullScreenID)
    {
        return false;
    }
    
    bool success = true;
    int numberOfScreens = getNumberOfScreens();
    int i = 0;
    while (i < numberOfScreens && success)
    {
        success = m_screens.at(i).getID() != screen.getID() && !(m_screens.at(i).isPrimary() && screen.isPrimary());
        ++i;
    }

    if (success)
    {
        m_screens << screen;
    }
    
    return success;
}

int ScreenLayout::getNumberOfScreens() const
{
    return m_screens.count();
}

void ScreenLayout::clear()
{
    m_screens.clear();
}

Screen ScreenLayout::getScreen(int screenID) const
{
    Screen screen;
    
    int index = getIndexOfScreen(screenID);
    if (index > -1)
    {
        screen = m_screens.at(index);
    }

    return screen;
}

int ScreenLayout::getPrimaryScreenID() const
{
    int id = -1;
    bool found = false;
    int numberOfScreens = getNumberOfScreens();
    int i = 0;
    while (i < numberOfScreens && !found)
    {
        if (m_screens.at(i).isPrimary())
        {
            id = m_screens.at(i).getID();
            found = true;
        }
        ++i;
    }

    return id;
}

int ScreenLayout::getScreenOnTheRightOf(int screenID)
{
    int rightScreen = Screen::NullScreenID;
    // Buscar una pantalla a la dreta i a la mateixa altura + o -
    for (int i = 0; i < getNumberOfScreens(); ++i)
    {
        // Si està a la dreta, però no està completament per sobre ni per sota
        if (isOnRight(i, screenID) && !isOver(i, screenID) && !isUnder(i, screenID))
        {
            // Si encara no hem trobat cap pantalla
            if (rightScreen == Screen::NullScreenID)
            {
                rightScreen = i;
            }
            // De les pantalles de la dreta, volem la més pròxima
            // Si la pantalla que hem trobat està més a l'esquerra que la que tenim
            else if (isOnLeft(i, rightScreen))
            {
                rightScreen = i;
            }
        }
    }

    return rightScreen;
}

int ScreenLayout::getScreenOnTheLeftOf(int screenID)
{
    int leftScreen = Screen::NullScreenID;
    // Buscar una pantalla a l'esquera i a la mateixa altura + o -
    for (int i = 0; i < getNumberOfScreens(); ++i)
    {
        // Si està a l'esquera, però no està completament per sobre ni per sota
        if (isOnLeft(i, screenID) && !isOver(i, screenID) && !isUnder(i, screenID))
        {
            // Si encara no hem trobat cap pantalla
            if (leftScreen == Screen::NullScreenID)
            {
                leftScreen = i;
            }
            // De les pantalles de l'esquera, volem la més pròxima
            // Si la pantalla que hem trobat està més a la dreta que la que tenim
            else if (isOnRight(i, leftScreen))
            {
                leftScreen = i;
            }
        }
    }

    return leftScreen;
}

bool ScreenLayout::isOver(int screen1, int screen2)
{
    if (getIndexOfScreen(screen1) == Screen::NullScreenID || getIndexOfScreen(screen2) == Screen::NullScreenID)
    {
        return false;
    }
    
    QRect screen1Geometry = getScreen(screen1).getGeometry();
    QRect screen2Geometry = getScreen(screen2).getGeometry();
    if (screen1Geometry.bottom() <= screen2Geometry.top())
    {
        return true;
    }
    return false;
}

bool ScreenLayout::isUnder(int screen1, int screen2)
{
    if (getIndexOfScreen(screen1) == Screen::NullScreenID || getIndexOfScreen(screen2) == Screen::NullScreenID)
    {
        return false;
    }

    QRect screen1Geometry = getScreen(screen1).getGeometry();
    QRect screen2Geometry = getScreen(screen2).getGeometry();
    if (screen1Geometry.top() >= screen2Geometry.bottom())
    {
        return true;
    }
    return false;
}

bool ScreenLayout::isOnLeft(int screen1, int screen2)
{
    if (getIndexOfScreen(screen1) == Screen::NullScreenID || getIndexOfScreen(screen2) == Screen::NullScreenID)
    {
        return false;
    }

    QRect screen1Geometry = getScreen(screen1).getGeometry();
    QRect screen2Geometry = getScreen(screen2).getGeometry();
    if (screen1Geometry.right() <= screen2Geometry.left())
    {
        return true;
    }
    return false;
}

bool ScreenLayout::isOnRight(int screen1, int screen2)
{
    if (getIndexOfScreen(screen1) == Screen::NullScreenID || getIndexOfScreen(screen2) == Screen::NullScreenID)
    {
        return false;
    }
    
    QRect screen1Geometry = getScreen(screen1).getGeometry();
    QRect screen2Geometry = getScreen(screen2).getGeometry();
    if (screen1Geometry.left() >= screen2Geometry.right())
    {
        return true;
    }
    return false;
}

int ScreenLayout::getIndexOfScreen(int screenID) const
{
    int i = 0;
    int numberOfScreens = getNumberOfScreens();
    while (i < numberOfScreens)
    {
        if (m_screens.at(i).getID() == screenID)
        {
            return i;
        }
        ++i;
    }

    return -1;
}

} // End namespace udg
