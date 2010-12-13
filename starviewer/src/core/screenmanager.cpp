/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "screenmanager.h"
#include "dynamicmatrix.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QWidget>
#include <QSize>

namespace udg {

ScreenManager::ScreenManager()
: SamePosition(5)
{
    m_applicationDesktop = QApplication::desktop();
}

void ScreenManager::maximize(QWidget *window)
{
    DynamicMatrix dynamicMatrix = computeScreenMatrix(window);    

    // agafa el top left i el bottomright per determinar les dimensions de la finestra
    QPoint topLeft = getTopLeft(dynamicMatrix);
    QPoint bottomRight = getBottomRight(dynamicMatrix);

    if (dynamicMatrix.isMaximizable())
    {
        if (window->isMaximized())
        {
            window->showNormal();
        }

        // Buscar la mida del frame i de la finestra
        QRect frameSize = window->frameGeometry();
        QRect windowSize = window->geometry();

        // Trobar el tamany real de les cantonades i la title bar
        int topBorder = windowSize.top() - frameSize.top();
        int bottomBorder = frameSize.bottom() - windowSize.bottom();
        int leftBorder = windowSize.left() - frameSize.left();
        int rightBorder = frameSize.right() - windowSize.right();

        // Calcular quin és el tamany que ha de tenir.
        // Se li ha de passar la geometria de la finestra, sense cantonades.
        int x = topLeft.x() + leftBorder;
        int y = topLeft.y() + topBorder;
        int width = bottomRight.x() - x - rightBorder; // x val x + leftBorder
        int height = bottomRight.y() - y - bottomBorder; // y val y + topBorder

        window->setGeometry(x, y, width, height);
    }
    else
    {
        window->showMaximized();
    }
}

void ScreenManager::moveToDesktop(QWidget *window, int idDesktop)
{
    if (doesItFitInto(window, idDesktop))
    {
        if (window->isMaximized())
        {
            window->showNormal();
            fitInto(window, idDesktop);
            window->showMaximized();
        }
        else
        {
            fitInto(window, idDesktop);
        }
    }
}

int ScreenManager::getNumberOfScreens()
{
    return m_applicationDesktop->numScreens();
}

DynamicMatrix ScreenManager::computeScreenMatrix(QWidget *window)
{
    int desktopIAm = m_applicationDesktop->screenNumber(window);

    //primer de tot buscar les pantalles de la mateixa fila
    DynamicMatrix dynamicMatrix;
    int indexLeft = 0;
    int indexRight = 0;
    int indexTop = 0;
    int indexBottom = 0;
    dynamicMatrix.setValue(0,0,desktopIAm);
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (int i = 0; i < m_applicationDesktop->numScreens(); i++)
        {
            if (isLeft(i, dynamicMatrix.getValue(0,indexLeft)))
            {
                dynamicMatrix.setValue(0,--indexLeft,i);
                changes = true;
            }
            else if (isRight(i, dynamicMatrix.getValue(0,indexRight)))
            {
                dynamicMatrix.setValue(0,++indexRight,i);
                changes = true;
            }
        }
    }
    //ara anar mirant si tota la fila te pantalles a sobre o a sota
    changes = true;
    bool keepLookingUp = true;
    bool keepLookingDown = true;
    while (changes)
    {
        changes = false;
        //mirar si es pot afegir la fila de sobre
        QList<int> topRow;
        int index = indexLeft;
        bool rowFound = keepLookingUp; //si hem de mirar la fila de sobre true altrament false
        while (keepLookingUp && index <= indexRight)
        {
            bool found = false;
            int j = 0;
            while (j < m_applicationDesktop->numScreens())
            {
                if (isTop(j, dynamicMatrix.getValue(indexTop,index)))
                {
                    topRow.append(j);
                    found = true;
                    j = m_applicationDesktop->numScreens();
                }
                j++;
            }
            if (found)
            {
                index++;
            }
            else
            {
                rowFound = false;
                keepLookingUp = false;
                index = indexRight + 1;
            }
        }
        //si s'ha pogut afegir TOTA la fila de sobre
        if (rowFound)
        {
            indexTop++;
            for (int i = indexLeft, j = 0; i <= indexRight; i++, j++)
            {
                dynamicMatrix.setValue(indexTop, i, topRow[j]);
            }
            changes = true;
        }

        //mirar si es pot afegir la fila de sota
        QList<int> bottomRow;
        index = indexLeft;
        rowFound = keepLookingDown; // si em de mirar la fila de sota true, altrament false
        while (keepLookingDown && index <= indexRight)
        {
            bool found = false;
            int j = 0;
            while (j < m_applicationDesktop->numScreens())
            {
                if (isBottom(j, dynamicMatrix.getValue(indexBottom,index)))
                {
                    bottomRow.append(j);
                    found = true;
                    j = m_applicationDesktop->numScreens();
                }
                j++;
            }
            if (found)
            {
                index++;
            }
            else
            {
                rowFound = false;
                keepLookingDown = false;
                index = indexRight + 1;
            }
        }
        //si s'ha pogut afegir TOTA la fila de sobre
        if (rowFound)
        {
            indexBottom--;
            for (int i = indexLeft, j = 0; i <= indexRight; i++, j++)
            {
                dynamicMatrix.setValue(indexBottom, i, bottomRow[j]);
            }
            changes = true;
        }
    }

    return dynamicMatrix;
}

bool ScreenManager::doesItFitInto(QWidget *window, int IdDesktop)
{

    int newDesktopWidth = m_applicationDesktop->availableGeometry(IdDesktop).width();
    int newDesktopHeight = m_applicationDesktop->availableGeometry(IdDesktop).height();

    // Si és massa ampla o massa alt, no hi cap.
    return !(newDesktopWidth < window->minimumWidth() || newDesktopHeight < window->minimumHeight());
}

void ScreenManager::fitInto(QWidget *window, int IdDesktop)
{
    int newDesktopWidth = m_applicationDesktop->availableGeometry(IdDesktop).width();
    int newDesktopHeight = m_applicationDesktop->availableGeometry(IdDesktop).height();

    int width = window->frameSize().width();
    int height = window->frameSize().height();
    int x = m_applicationDesktop->availableGeometry(IdDesktop).topLeft().x();
    int y = m_applicationDesktop->availableGeometry(IdDesktop).topLeft().y();

    // Buscar la mida del frame i de la finestra
    QRect frameSize = window->frameGeometry();
    QRect windowSize = window->geometry();

    // Trobar el tamany real de les cantonades i la title bar
    int topBorder = windowSize.top() - frameSize.top();
    int bottomBorder = frameSize.bottom() - windowSize.bottom();
    int leftBorder = windowSize.left() - frameSize.left();
    int rightBorder = frameSize.right() - windowSize.right();

    // Si la finestra és més ample que la pantalla
    if (width > newDesktopWidth)
    {
        width = newDesktopWidth;
        x += leftBorder;
    }
    else // altrament centrar
    {
        x = x + newDesktopWidth / 2 - width / 2 + leftBorder;
    }

    // Si la finestra és més alta que la pantalla
    if (height > newDesktopHeight)
    {
        height = newDesktopHeight;
        y += topBorder;
    }
    else // altrament centrar
    {
        y = y + newDesktopHeight / 2 - height / 2 + topBorder;
    }

    // La mida de la finestra l'hem agafat del frame, per tant li hem de treure
    // les cantonades per que la mida sigui la mateixa
    window->setGeometry(x, 
                        y, 
                        width - leftBorder - rightBorder, 
                        height - topBorder - bottomBorder);
}

bool ScreenManager::isTop(int desktop1, int desktop2)
{
    // esta posat a sobre
    if (abs(m_applicationDesktop->screenGeometry(desktop1).bottom() - m_applicationDesktop->screenGeometry(desktop2).top()) < SamePosition)
    {
        // te la mateixa alçada
        int leftPart = abs(m_applicationDesktop->screenGeometry(desktop1).left() - m_applicationDesktop->screenGeometry(desktop2).left());
        int rightPart = abs(m_applicationDesktop->screenGeometry(desktop1).right() - m_applicationDesktop->screenGeometry(desktop2).right());
        if (leftPart + rightPart < SamePosition)
        {
            return true;
        }
    }

    return false;
}

bool ScreenManager::isBottom(int desktop1, int desktop2)
{
    // esta posat a sota
    if (abs(m_applicationDesktop->screenGeometry(desktop1).top() - m_applicationDesktop->screenGeometry(desktop2).bottom()) < SamePosition)
    {
        // te la mateixa alçada
        int leftPart = abs(m_applicationDesktop->screenGeometry(desktop1).left() - m_applicationDesktop->screenGeometry(desktop2).left());
        int rightPart = abs(m_applicationDesktop->screenGeometry(desktop1).right() - m_applicationDesktop->screenGeometry(desktop2).right());
        if (leftPart + rightPart < SamePosition)
        {
            return true;
        }
    }

    return false;
}

bool ScreenManager::isLeft(int desktop1, int desktop2)
{
    // esta posat a l'esquerra
    if (abs(m_applicationDesktop->screenGeometry(desktop1).right() - m_applicationDesktop->screenGeometry(desktop2).left()) < SamePosition)
    {
        // te la mateixa alçada
        int topPart = abs(m_applicationDesktop->screenGeometry(desktop1).top() - m_applicationDesktop->screenGeometry(desktop2).top());
        int bottomPart = abs(m_applicationDesktop->screenGeometry(desktop1).bottom() - m_applicationDesktop->screenGeometry(desktop2).bottom());
        if (topPart + bottomPart < SamePosition)
        {
            return true;
        }
    }
    return false;
}

bool ScreenManager::isRight(int desktop1, int desktop2)
{
    // esta posat a l'esquerra
    if (abs(m_applicationDesktop->screenGeometry(desktop1).left() - m_applicationDesktop->screenGeometry(desktop2).right()) < SamePosition)
    {
        // te la mateixa alçada
        int topPart = abs(m_applicationDesktop->screenGeometry(desktop1).top() - m_applicationDesktop->screenGeometry(desktop2).top());
        int bottomPart = abs(m_applicationDesktop->screenGeometry(desktop1).bottom() - m_applicationDesktop->screenGeometry(desktop2).bottom());
        if (topPart + bottomPart < SamePosition)
        {
            return true;
        }
    }
    return false;
}

bool ScreenManager::isTopLeft(int desktop1, int desktop2)
{
    int x = abs(m_applicationDesktop->screenGeometry(desktop1).bottomRight().x() - m_applicationDesktop->screenGeometry(desktop2).topLeft().x());
    int y = abs(m_applicationDesktop->screenGeometry(desktop1).bottomRight().y() - m_applicationDesktop->screenGeometry(desktop2).topLeft().y());
    if (abs(x*x - y*y) < SamePosition)
    {
        return true;
    }
    return false;
}

bool ScreenManager::isTopRight(int desktop1, int desktop2)
{
    int x = abs(m_applicationDesktop->screenGeometry(desktop1).bottomLeft().x() - m_applicationDesktop->screenGeometry(desktop2).topRight().x());
    int y = abs(m_applicationDesktop->screenGeometry(desktop1).bottomLeft().y() - m_applicationDesktop->screenGeometry(desktop2).topRight().y());
    if (abs(x*x - y*y) < SamePosition)
    {
        return true;
    }
    return false;
}

bool ScreenManager::isBottomLeft(int desktop1, int desktop2)
{
    int x = abs(m_applicationDesktop->screenGeometry(desktop1).topRight().x() - m_applicationDesktop->screenGeometry(desktop2).bottomLeft().x());
    int y = abs(m_applicationDesktop->screenGeometry(desktop1).topRight().y() - m_applicationDesktop->screenGeometry(desktop2).bottomLeft().y());
    if (abs(x*x - y*y) < SamePosition)
    {
        return true;
    }
    return false;
}

bool ScreenManager::isBottomRight(int desktop1, int desktop2)
{

    int x = abs(m_applicationDesktop->screenGeometry(desktop1).topLeft().x() - m_applicationDesktop->screenGeometry(desktop2).bottomRight().x());
    int y = abs(m_applicationDesktop->screenGeometry(desktop1).topLeft().y() - m_applicationDesktop->screenGeometry(desktop2).bottomRight().y());
    if (abs(x*x - y*y) < SamePosition)
    {
        return true;
    }
    return false;
}

int ScreenManager::whoIsLeft(int desktopIAm)
{
    for(int i = 0; i < m_applicationDesktop->numScreens(); i++)
    {
        if (isLeft(i, desktopIAm))
        {
            return i;
        }
    }

    return -1;
}

int ScreenManager::whoIsRight(int desktopIAm)
{
    for(int i = 0; i < m_applicationDesktop->numScreens(); i++)
    {
        if (isRight(i, desktopIAm))
        {
            return i;
        }
    }

    return -1;
}

int ScreenManager::whoIsTop(int desktopIAm)
{
    for(int i = 0; i < m_applicationDesktop->numScreens(); i++)
    {
        if (isTop(i, desktopIAm))
        {
            return i;
        }
    }

    return -1;
}

int ScreenManager::whoIsBottom(int desktopIAm)
{

    for(int i = 0; i < m_applicationDesktop->numScreens(); i++)
    {
        if (isBottom(i, desktopIAm))
        {
            return i;
        }
    }

    return -1;
}

QPoint ScreenManager::getTopLeft(const DynamicMatrix &dynamicMatrix) const
{
    //Primer de tot buscar la cantonada esquerra, a partir de la llista de monitors a l'esquerra,
    //agafar el màxim, per si la barra de windows esta a l'esquerra en algun d'ells
    QList<int> screens = dynamicMatrix.getLeftColumn();
    int x = m_applicationDesktop->availableGeometry(screens[0]).left();
    for (int i = 1; i < screens.count(); i++)
    {
        x = std::max(x, m_applicationDesktop->availableGeometry(screens[i]).left());
    }
    //el mateix per la part superior
    screens = dynamicMatrix.getTopRow();
    int y = m_applicationDesktop->availableGeometry(screens[0]).top();
    for (int i = 1; i < screens.count(); i++)
    {
        y = std::max(y, m_applicationDesktop->availableGeometry(screens[i]).top());
    }

    return QPoint(x,y);
}

QPoint ScreenManager::getBottomRight(const DynamicMatrix &dynamicMatrix) const
{

    //Primer de tot buscar la cantonada dreta, a partir de la llista de monitors a lla dreta,
    //agafar el mínim, per si la barra de windows esta a la dreta en algun d'ells
    QList<int> screens = dynamicMatrix.getRightColumn();
    int x = m_applicationDesktop->availableGeometry(screens[0]).right();
    for (int i = 1; i < screens.count(); i++)
    {
        x = std::min(x, m_applicationDesktop->availableGeometry(screens[i]).right());
    }
    //el mateix per la part de baix
    screens = dynamicMatrix.getBottomRow();
    int y = m_applicationDesktop->availableGeometry(screens[0]).bottom();
    for (int i = 1; i < screens.count(); i++)
    {
        y = std::min(y, m_applicationDesktop->availableGeometry(screens[i]).bottom());
    }

    return QPoint(x,y);
}

} // end namespace udg
