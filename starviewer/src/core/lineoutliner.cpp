/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "lineoutliner.h"
#include "q2dviewer.h"
#include "drawer.h"
#include "drawerline.h"

// Vtk's
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

LineOutliner::LineOutliner(Q2DViewer *viewer, QObject *parent)
 : Outliner(viewer, parent)
{
    m_lineState = NoPoints;
}

LineOutliner::~LineOutliner()
{
}

void LineOutliner::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            if (m_2DViewer->getInput())
            {
                if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
                {
                    this->annotateNewPoint();
                    m_2DViewer->render();
                    if (m_lineState == Finished)
                    {
                        emit finished(m_line);
                    }
                }
            }
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_line)
            {
                this->simulateLine();
                m_2DViewer->getDrawer()->updateRenderer();
            }
            break;
    }
}

void LineOutliner::annotateNewPoint()
{
    if (!m_line)
    {
        m_line = new DrawerLine;
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    // Afegim el punt
    if (m_lineState == NoPoints)
    {
        m_line->setFirstPoint(clickedWorldPoint);
        m_line->setSecondPoint(clickedWorldPoint);
        m_lineState = FirstPoint;

        m_2DViewer->getDrawer()->drawWorkInProgress(m_line);
    }
    else
    {
        m_line->setSecondPoint(clickedWorldPoint);
        m_line->update();
        m_lineState = Finished;
    }
}

void LineOutliner::simulateLine()
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    m_line->setSecondPoint(clickedWorldPoint);
    m_line->update();

    m_2DViewer->render();
}

}
