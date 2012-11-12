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

int ScreenLayout::getScreenOnTheRightOf(int screenID) const
{
    int rightScreenID = Screen::NullScreenID;
    Screen screen = getScreen(screenID);
    Screen screenToCompare;
    // Buscar una pantalla a la dreta i a la mateixa altura + o -
    for (int i = 0; i < getNumberOfScreens(); ++i)
    {
        screenToCompare = getScreen(i);
        // Si està a la dreta, però no està completament per sobre ni per sota
        if (screenToCompare.isOnRight(screen) && !screenToCompare.isOver(screen) && !screenToCompare.isUnder(screen))
        {
            // Si encara no hem trobat cap pantalla
            if (rightScreenID == Screen::NullScreenID)
            {
                rightScreenID = i;
            }
            // De les pantalles de la dreta, volem la més pròxima
            // Si la pantalla que hem trobat està més a l'esquerra que la que tenim
            else if (screenToCompare.isOnLeft(getScreen(rightScreenID)))
            {
                rightScreenID = i;
            }
        }
    }

    return rightScreenID;
}

int ScreenLayout::getScreenOnTheLeftOf(int screenID) const
{
    int leftScreenID = Screen::NullScreenID;
    Screen screen = getScreen(screenID);
    Screen screenToCompare;
    // Buscar una pantalla a l'esquera i a la mateixa altura + o -
    for (int i = 0; i < getNumberOfScreens(); ++i)
    {
        screenToCompare = getScreen(i);
        // Si està a l'esquera, però no està completament per sobre ni per sota
        if (screenToCompare.isOnLeft(screen) && !screenToCompare.isOver(screen) && !screenToCompare.isUnder(screen))
        {
            // Si encara no hem trobat cap pantalla
            if (leftScreenID == Screen::NullScreenID)
            {
                leftScreenID = i;
            }
            // De les pantalles de l'esquera, volem la més pròxima
            // Si la pantalla que hem trobat està més a la dreta que la que tenim
            else if (screenToCompare.isOnRight(getScreen(leftScreenID)))
            {
                leftScreenID = i;
            }
        }
    }

    return leftScreenID;
}

int ScreenLayout::getPreviousScreenOf(int screenID) const
{
    if (getIndexOfScreen(screenID) < 0)
    {
        return -1;
    }
    
    Screen currentScreen = getScreen(screenID);

    int previousScreenID = getScreenOnTheLeftOf(screenID);
    // Si no hi ha cap pantalla a l'esquerra, llavors busquem la de més a la dreta que està per sobre d'aquesta
    if (previousScreenID == -1)
    {
        Screen screen;
        for (int i = 0; i < getNumberOfScreens(); i++)
        {
            screen = getScreen(i);
            if (screen.isHigher(currentScreen))
            {
                // Si encara no hem trobat cap pantalla
                if (previousScreenID == -1)
                {
                    previousScreenID = i;
                }
                // De les pantalles de sobre, volem la més a la dreta
                else if (screen.isOnRight(getScreen(previousScreenID)))
                {
                    previousScreenID = i;
                }
            }
        }
    }

    // Si no hi ha cap pantalla per sobre de la actual, agafarem la de més avall a la dreta
    if (previousScreenID == -1)
    {
        Screen screen;
        // Amb això assegurem que mai arribarà al moveToDesktop valent -1
        previousScreenID = 0;
        for (int i = 1; i < getNumberOfScreens(); i++)
        {
            screen = getScreen(i);
            // Si està per sota de l'actual ens la quedem
            if (screen.isUnder(getScreen(previousScreenID)))
            {
                previousScreenID = i;
            }
            // Si no, si no està per sobre, l'agafem si està més a la dreta que l'actual
            else
            {
                Screen previousScreen = getScreen(previousScreenID);
                if (!screen.isOver(previousScreen) && screen.isMoreToTheRight(previousScreen))
                {
                    previousScreenID = i;
                }
            }
        }
    }

    return previousScreenID;
}

int ScreenLayout::getNextScreenOf(int screenID) const
{
    if (getIndexOfScreen(screenID) < 0)
    {
        return -1;
    }
    
    Screen currentScreen = getScreen(screenID);

    // Buscar una pantalla a la dreta i a la mateixa altura + o -
    int nextScreenID = getScreenOnTheRightOf(screenID);
    
    // Si no hi ha cap pantalla a la dreta, llavors busquem la de més a l'esquerra que està per sota d'aquesta
    if (nextScreenID == -1)
    {
        Screen screen;
        for (int i = 0; i < getNumberOfScreens(); i++)
        {
            screen = getScreen(i);
            if (screen.isLower(currentScreen))
            {
                // Si encara no hem trobat cap pantalla
                if (nextScreenID == -1)
                {
                    nextScreenID = i;
                }
                // De les pantalles de sota, volem la més a l'esquerra
                else if (screen.isOnLeft(getScreen(nextScreenID)))
                {
                    nextScreenID = i;
                }
            }
        }
    }

    // Si no hi ha cap patalla per sota de la actual, agafarem la de més amunt a l'esquerra
    if (nextScreenID == -1)
    {
        Screen screen;
        // Amb això assegurem que mai arribarà al moveToDesktop valent -1
        nextScreenID = 0;
        for (int i = 1; i < getNumberOfScreens(); i++)
        {
            screen = getScreen(i);
            // Si està per sobre de l'actual ens la quedem
            if (screen.isOver(getScreen(nextScreenID)))
            {
                nextScreenID = i;
            }
            // Si no, si no està per sota, l'agafem si està més a l'esquerra que l'actual
            else
            {
                Screen nextScreen = getScreen(nextScreenID);
                if (!screen.isUnder(nextScreen) && screen.isMoreToTheLeft(nextScreen))
                {
                    nextScreenID = i;
                }
            }
        }
    }

    return nextScreenID;
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
