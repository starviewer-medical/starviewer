/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "nonclosedangletoolrepresentation.h"

#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"
#include "q2dviewer.h"
#include "mathtools.h"
#include "toolhandlerwithrepresentation.h"
#include "toolhandlerwithoutrepresentation.h"

namespace udg {

NonClosedAngleToolRepresentation::NonClosedAngleToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent)
{
}

NonClosedAngleToolRepresentation::~NonClosedAngleToolRepresentation()
{
    delete m_firstLine;
    delete m_secondLine;
    if (m_middleLine)
    {
        delete m_middleLine;
    }
    delete m_text;

    foreach (ToolHandler *handler, m_toolHandlers)
    {
        delete handler;
    }

    m_toolHandlers.clear();
    m_toolHandlersMap.clear();
}

void NonClosedAngleToolRepresentation::setLines(DrawerLine *firstLine, DrawerLine *secondLine)
{
    m_firstLine = firstLine;
    m_secondLine = secondLine;
    m_middleLine = new DrawerLine;
    m_text = new DrawerText;
    m_primitivesList << m_firstLine << m_secondLine << m_middleLine << m_text;
}

void NonClosedAngleToolRepresentation::setParams(int view, int slice)
{
    m_view = view;
    m_slice = slice;
}

void NonClosedAngleToolRepresentation::calculate()
{
    this->computeAngle();

    if (m_toolHandlers.size() == 0) // Already created
    {
        this->createHandlers();
    }
}

void NonClosedAngleToolRepresentation::computeAngle()
{
    m_middleLine->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);

    double *p1 = m_firstLine->getFirstPoint();
    double *p2 = m_firstLine->getSecondPoint();
    double *p3 = m_secondLine->getFirstPoint();
    double *p4 = m_secondLine->getSecondPoint();

    double *vd1, *vd2;

    double *intersection;
    int state;

    intersection = MathTools::infiniteLinesIntersection(p1, p2, p3, p4, state);

    double dist1, dist2, dist3, dist4;
    dist1 = MathTools::getDistance3D(intersection, p1);
    dist2 = MathTools::getDistance3D(intersection, p2);
    dist3 = MathTools::getDistance3D(intersection, p3);
    dist4 = MathTools::getDistance3D(intersection, p4);

    if (dist1 <= dist2)
    {
        if (dist3 <= dist4)
        {
            vd1 = MathTools::directorVector(p1, intersection);
            vd2 = MathTools::directorVector(p3, intersection);
            m_middleLine->setFirstPoint(p1);
            m_middleLine->setSecondPoint(p3);
        }
        else
        {
            vd1 = MathTools::directorVector(p1, intersection);
            vd2 = MathTools::directorVector(p4, intersection);
            m_middleLine->setFirstPoint(p1);
            m_middleLine->setSecondPoint(p4);
        }
    }
    else
    {
        if (dist3 <= dist4)
        {
            vd1 = MathTools::directorVector(p2, intersection);
            vd2 = MathTools::directorVector(p3, intersection);
            m_middleLine->setFirstPoint(p2);
            m_middleLine->setSecondPoint(p3);
        }
        else
        {
            vd1 = MathTools::directorVector(p2, intersection);
            vd2 = MathTools::directorVector(p4, intersection);
            m_middleLine->setFirstPoint(p2);
            m_middleLine->setSecondPoint(p4);
        }
    }

    // Dibuixem la línia auxiliar
    m_drawer->draw(m_middleLine);

    for (int i = 0; i < 3; i++)
    {
        if (fabs(vd1[i]) < 0.0001)
        {
            vd1[i] = 0.0;
        }

        if (fabs(vd2[i]) < 0.0001)
        {
            vd2[i] = 0.0;
        }
    }

    double angle = MathTools::angleInDegrees(vd1, vd2);

    if (state == MathTools::ParallelLines)
    {
        m_text->setText(tr("0.0 degrees"));
    }
    else if (state == MathTools::SkewIntersection) // Won't occur
    {
        m_text->setText(tr("Skew lines."));
    }
    else
    {
        m_text->setText(tr("%1 degrees").arg(angle,0,'f',1));
    }

    textPosition(m_middleLine->getFirstPoint(), m_middleLine->getSecondPoint());

    m_text->update();
    m_drawer->draw(m_text);
    m_drawer->updateRenderer();
}

void NonClosedAngleToolRepresentation::textPosition(double *p1, double *p2)
{
    double position[3];
    int horizontalCoord = Q2DViewer::getXIndexForView(m_view);
    int verticalCoord = Q2DViewer::getYIndexForView(m_view);

    position[horizontalCoord] = (p1[horizontalCoord] + p2[horizontalCoord]) / 2.0;
    position[verticalCoord] = (p1[verticalCoord] + p2[verticalCoord]) / 2.0;

    m_text->setAttachmentPoint(position);
}

void NonClosedAngleToolRepresentation::createHandlers()
{
    // HACK
    int zCoordinate = Q2DViewer::getZIndexForView(m_view);
    double depth = m_firstLine->getFirstPoint()[zCoordinate];

    ToolHandlerWithRepresentation *firstHandler = new ToolHandlerWithRepresentation(m_drawer, m_firstLine->getFirstPoint());
    ToolHandlerWithRepresentation *secondHandler = new ToolHandlerWithRepresentation(m_drawer, m_firstLine->getSecondPoint());
    ToolHandlerWithRepresentation *thirdHandler = new ToolHandlerWithRepresentation(m_drawer, m_secondLine->getFirstPoint());
    ToolHandlerWithRepresentation *fourthHandler = new ToolHandlerWithRepresentation(m_drawer, m_secondLine->getSecondPoint());

    m_toolHandlersMap.insert(1, firstHandler);
    m_toolHandlersMap.insert(2, secondHandler);
    m_toolHandlersMap.insert(3, thirdHandler);
    m_toolHandlersMap.insert(4, fourthHandler);

    connect(firstHandler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
    connect(firstHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
    connect(this, SIGNAL(hideToolHandlers()), firstHandler, SLOT(hide()));
    connect(this, SIGNAL(showToolHandlers()), firstHandler, SLOT(show()));
    connect(secondHandler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
    connect(secondHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
    connect(this, SIGNAL(hideToolHandlers()), secondHandler, SLOT(hide()));
    connect(this, SIGNAL(showToolHandlers()), secondHandler, SLOT(show()));
    connect(thirdHandler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
    connect(thirdHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
    connect(this, SIGNAL(hideToolHandlers()), thirdHandler, SLOT(hide()));
    connect(this, SIGNAL(showToolHandlers()), thirdHandler, SLOT(show()));
    connect(fourthHandler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
    connect(fourthHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
    connect(this, SIGNAL(hideToolHandlers()), fourthHandler, SLOT(hide()));
    connect(this, SIGNAL(showToolHandlers()), fourthHandler, SLOT(show()));


    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), firstHandler, SLOT(move(double *)));
    connect(toolHandler, SIGNAL(moveAllPoints(double *)), secondHandler, SLOT(move(double *)));
    connect(toolHandler, SIGNAL(moveAllPoints(double *)), thirdHandler, SLOT(move(double *)));
    connect(toolHandler, SIGNAL(moveAllPoints(double *)), fourthHandler, SLOT(move(double *)));

    firstHandler->setParams(zCoordinate, depth);
    secondHandler->setParams(zCoordinate, depth);
    thirdHandler->setParams(zCoordinate, depth);
    fourthHandler->setParams(zCoordinate, depth);
    toolHandler->setParams(zCoordinate, depth);

    m_toolHandlers << firstHandler << secondHandler << thirdHandler << fourthHandler << toolHandler;
}

QList<double *> NonClosedAngleToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    QList<double *> list;
    const int key = m_toolHandlersMap.key(toolHandler);

    switch (key)
    {
        case 1:
            list << m_firstLine->getFirstPoint();
            break;
        case 2:
            list << m_firstLine->getSecondPoint();
            break;
        case 3:
            list << m_secondLine->getFirstPoint();
            break;
        case 4:
            list << m_secondLine->getSecondPoint();
            break;
    }

    return list;
}

void NonClosedAngleToolRepresentation::moveAllPoints(double *movement)
{
    QList<double *> points;
    points << m_firstLine->getFirstPoint() << m_firstLine->getSecondPoint() << m_secondLine->getFirstPoint() << m_secondLine->getSecondPoint()
        << m_middleLine->getFirstPoint() << m_middleLine->getSecondPoint() << m_text->getAttachmentPoint();

    foreach (double *point, points)
    {
        point[0] += movement[0];
        point[1] += movement[1];
        point[2] += movement[2];
    }

    this->refresh();
}

}
