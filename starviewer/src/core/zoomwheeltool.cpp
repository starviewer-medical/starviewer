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

#include "zoomwheeltool.h"
#include "qviewer.h"
#include "logging.h"

// Vtk
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

// Qt
#include <QTimer>
#include <QTimeLine>

namespace udg {

ZoomWheelTool::ZoomWheelTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_ignoreWheelMovement(false), m_increment(0)
{
    m_toolName = "ZoomWheelTool";

    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->setInterval(275); // Human vision reaction time plus a margin http://www.humanbenchmark.com/tests/reactiontime/
    connect(m_timer, SIGNAL(timeout()), this, SLOT(resetTool()));

    // Ensure that we have a valid viewer from the beginning (constructor)
    Q_ASSERT(m_viewer);
}

ZoomWheelTool::~ZoomWheelTool()
{
    delete m_timer;
}

void ZoomWheelTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::MouseWheelForwardEvent:
            onWheelMoved(m_viewer->getWheelAngleDelta().y());
        break;

        case vtkCommand::MouseWheelBackwardEvent:
            onWheelMoved(m_viewer->getWheelAngleDelta().y());
        break;

        case vtkCommand::MiddleButtonPressEvent:
            m_ignoreWheelMovement = true;
        break;

        case vtkCommand::MiddleButtonReleaseEvent:
            m_ignoreWheelMovement = false;
        break;

        default:
        break;
    }
}

void ZoomWheelTool::resetTool()
{
    m_viewer->unsetCursor();
}

void ZoomWheelTool::scalingTime()
{
    // Increase/decrease motion factor to increase/decrease zoom speed (never <= 0)
    // Increments divided by a large number (far from approximate maximum number of increments)
    // Factor depends on how much we want to zoom the scene. If user wants to zoom it just by a little bit, he will touch mousewheel very delicately:
    //  m_increment will be small, and so will be factor. On the other hand if user rotates the wheel intensively, m_increment will be bigger,
    //  and so we will zoom the scene faster.
    // Source: https://wiki.qt.io/Smooth_Zoom_In_QGraphicsView
    constexpr double MotionFactor = 1.0;
    double factor = 1.0 + MotionFactor * double(m_increment) / 300.0;
    m_viewer->zoom(factor, m_zoomCenter);
}

void ZoomWheelTool::animFinished()
{
    // Increase or decrease increments depending on wheel direction
    if (m_increment > 0) m_increment--;
    else m_increment++;

    // Delete animation (dynamic memory)
    sender()->~QObject();
}

void ZoomWheelTool::onWheelMoved(int angleDelta)
{
    m_timer->stop();

    if (!m_ignoreWheelMovement)
    {
        m_viewer->setCursor(QCursor(QPixmap(":/images/cursors/zoom.svg")));
        m_zoomCenter = m_viewer->getEventPosition();

        // Compute wheel increments
        int increment = angleDelta / 120;
        m_increment += increment;

        // If user moved the wheel in another direction, ignore pending increments and set previously computed ones
        if (m_increment * increment < 0) m_increment = increment;

        // Set duration of the zoom process (total duration) and interval (how often a new render happens)
        QTimeLine *anim = new QTimeLine(350, this);
        anim->setUpdateInterval(20);

        // Slots executed for each animation interval and at the end
        connect(anim, SIGNAL(valueChanged(qreal)), SLOT(scalingTime()));
        connect(anim, SIGNAL(finished()), SLOT(animFinished()));
        anim->start();
    }

    m_timer->start();
}

}
