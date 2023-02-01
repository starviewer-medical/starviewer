/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "arrowtool.h"

#include "drawer.h"
#include "drawerarrow.h"
#include "imageplane.h"
#include "q2dviewer.h"
#include "volume.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

ArrowTool::ArrowTool(QViewer *viewer, QObject *parent)
    : Tool(viewer, parent), m_drawing(false)
{
    m_toolName = "ArrowTool";
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    connect(m_2DViewer, &Q2DViewer::volumeChanged, this, &ArrowTool::initialize);
    initialize();
}

ArrowTool::~ArrowTool()
{
    initialize();
}

void ArrowTool::handleEvent(unsigned long eventId)
{
    if (!m_2DViewer || !m_2DViewer->hasInput())
    {
        return;
    }

    switch (eventId)
    {
        case vtkCommand::LeftButtonPressEvent:
            onClick();
            break;
        case vtkCommand::MouseMoveEvent:
            onMouseMove();
            break;
        case vtkCommand::KeyPressEvent:
            onKeyPress();
            break;
    }
}

void ArrowTool::initialize()
{
    if (m_arrow)
    {
        m_arrow->decreaseReferenceCount();
        delete m_arrow;
    }

    m_arrow = nullptr;
    m_drawing = false;
}

void ArrowTool::onClick()
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    Vector3 point(clickedWorldPoint[0], clickedWorldPoint[1], clickedWorldPoint[2]);

    if (!m_drawing)
    {
        ImagePlane *imagePlane = m_2DViewer->getCurrentImagePlane();
        m_arrow = new DrawerArrow();
        m_arrow->increaseReferenceCount();
        m_arrow->setStartPoint(point);
        if (imagePlane)
        {
            m_arrow->setScale(std::max(imagePlane->getRowLength(), imagePlane->getColumnLength()));
        }
        delete imagePlane;
        m_drawing = true;

        m_2DViewer->getDrawer()->draw(m_arrow);
    }
    else
    {
        m_arrow->setEndPoint(point);
        m_arrow->setViewPlaneNormal(m_2DViewer->getViewPlaneNormal());
        equalizeDepth();
        m_arrow->update();

        m_arrow->decreaseReferenceCount();
        m_2DViewer->getDrawer()->erasePrimitive(m_arrow);

        m_2DViewer->getDrawer()->draw(m_arrow, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

        m_drawing = false;
        m_arrow = nullptr;
    }
}

void ArrowTool::onMouseMove()
{
    if (m_drawing)
    {
        double mouseWorldPoint[3];
        m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);
        Vector3 point(mouseWorldPoint[0], mouseWorldPoint[1], mouseWorldPoint[2]);

        m_arrow->setEndPoint(point);
        m_arrow->setViewPlaneNormal(m_2DViewer->getViewPlaneNormal());
        equalizeDepth();
        m_arrow->update();

        m_2DViewer->render();
    }
}

void ArrowTool::onKeyPress()
{
    int keyCode = m_viewer->getInteractor()->GetKeyCode();

    if (keyCode == 27)  // Esc
    {
        initialize();
    }
}

void ArrowTool::equalizeDepth()
{
    // Set the Z of the start point to the Z of the end point
    int zIndex = m_2DViewer->getView().getZIndex();
    double z = m_arrow->getEndPoint().toArray()[zIndex];
    std::array<double, 3> startPoint = m_arrow->getStartPoint();
    startPoint[zIndex] = z;
    m_arrow->setStartPoint(startPoint);
    m_arrow->update();
}

} // namespace udg
