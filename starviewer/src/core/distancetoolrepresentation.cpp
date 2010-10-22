/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetoolrepresentation.h"

#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"
#include "toolhandlerwithrepresentation.h"
#include "toolhandlerwithoutrepresentation.h"

#include "q2dviewer.h"

namespace udg {

DistanceToolRepresentation::DistanceToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent)
{
}

DistanceToolRepresentation::~DistanceToolRepresentation()
{
    delete m_line;
    delete m_text;

    foreach (ToolHandler *handler, m_toolHandlers)
    {
        delete handler;
    }

    m_toolHandlers.clear();
    m_toolHandlersMap.clear();
}

void DistanceToolRepresentation::setLine(DrawerLine *line)
{
    m_line = line;
    m_text = new DrawerText;
    m_primitivesList << m_line << m_text;
}

void DistanceToolRepresentation::setParams(int view, const double *pixelSpacing, double *spacing)
{
    m_view = view;
    m_pixelSpacing = pixelSpacing;
    m_spacing = spacing;
}

void DistanceToolRepresentation::calculate()
{
    if (m_pixelSpacing[0] == 0.0 && m_pixelSpacing[1] == 0.0)
    {
        m_text->setText(tr("%1 px").arg(m_line->computeDistance(m_spacing), 0, 'f', 0));
    }
    else
    {
        m_text->setText(tr("%1 mm").arg(m_line->computeDistance(), 0, 'f', 2));
    }
    m_text->setAttachmentPoint(m_line->getLeftPoint(m_view));
    m_text->setHorizontalJustification("Right");
    m_text->update();
    m_drawer->draw(m_text);
    m_drawer->updateRenderer();

    if (m_toolHandlers.size() == 0) // Already created
    {
        this->createHandlers();
    }
}

void DistanceToolRepresentation::createHandlers()
{
    // HACK
    int zCoordinate = Q2DViewer::getZIndexForView(m_view);
    double depth = m_line->getFirstPoint()[zCoordinate];

    ToolHandlerWithRepresentation *firstHandler = new ToolHandlerWithRepresentation(m_drawer, m_line->getFirstPoint(), m_handlerColor);
    ToolHandlerWithRepresentation *secondHandler = new ToolHandlerWithRepresentation(m_drawer, m_line->getSecondPoint());

    m_toolHandlersMap.insert(1, firstHandler);
    m_toolHandlersMap.insert(2, secondHandler);

    connect(firstHandler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
    connect(this, SIGNAL(hideToolHandlers()), firstHandler, SLOT(hide()));
    connect(this, SIGNAL(showToolHandlers()), firstHandler, SLOT(show()));
    connect(firstHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    connect(secondHandler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
    connect(this, SIGNAL(hideToolHandlers()), secondHandler, SLOT(hide()));
    connect(this, SIGNAL(showToolHandlers()), secondHandler, SLOT(show()));
    connect(secondHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), firstHandler, SLOT(move(double *)));
    connect(toolHandler, SIGNAL(moveAllPoints(double *)), secondHandler, SLOT(move(double *)));

    firstHandler->setParams(zCoordinate, depth);
    secondHandler->setParams(zCoordinate, depth);
    toolHandler->setParams(zCoordinate, depth);

    m_toolHandlers << firstHandler << secondHandler << toolHandler;
}

QList<double *> DistanceToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    QList<double *> list;

    const int key = m_toolHandlersMap.key(toolHandler);

    switch (key)
    {
        case 1:
            list << m_line->getFirstPoint();
            break;
        case 2:
            list << m_line->getSecondPoint();
            break;
    }

    return list;
}

void DistanceToolRepresentation::moveAllPoints(double *movement)
{
    QList<double *> points;
    points << m_line->getFirstPoint() << m_line->getSecondPoint() << m_text->getAttachmentPoint();

    foreach (double *point, points)
    {
        point[0] += movement[0];
        point[1] += movement[1];
        point[2] += movement[2];
    }

    this->refresh();
}

}
