/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "angletoolrepresentation.h"

#include "drawer.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "q2dviewer.h"
#include "mathtools.h"
#include "toolhandlerwithrepresentation.h"
#include "toolhandlerwithoutrepresentation.h"

namespace udg {

AngleToolRepresentation::AngleToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent)
{
}

AngleToolRepresentation::~AngleToolRepresentation()
{
    delete m_polyline;
    delete m_text;

    foreach (ToolHandler *handler, m_toolHandlers)
    {
        delete handler;
    }

    m_toolHandlers.clear();
    m_toolHandlersMap.clear();
}

void AngleToolRepresentation::setPolyline(DrawerPolyline *polyline)
{
    m_polyline = polyline;
    m_text = new DrawerText;
    m_primitivesList << m_polyline << m_text;
}

void AngleToolRepresentation::setParams(int view, int slice, double degrees)
{
    m_view = view;
    m_slice = slice;
    m_currentAngle = degrees;

    m_edited = false;
}

void AngleToolRepresentation::calculate()
{
    //if (m_edited)
    recalculateAngle();

    delete m_text;
    m_primitivesList.removeOne(m_text);
    m_text = new DrawerText;
    m_primitivesList << m_polyline << m_text;
    m_text->setColor(m_selectedRepresentationColor);

    // Afegim l'annotació textual
    m_text->setText(tr("%1 degrees").arg(m_currentAngle,0,'f',1));
    placeText();
    m_drawer->draw(m_text);
    m_drawer->updateRenderer();

    if (m_toolHandlers.size() == 0) // Already created
    {
        this->createHandlers(); 
    }
}

void AngleToolRepresentation::recalculateAngle()
{
    double *vd1, *vd2;

    vd1 = MathTools::directorVector(m_polyline->getPoint(0), m_polyline->getPoint(1));
    vd2 = MathTools::directorVector(m_polyline->getPoint(2), m_polyline->getPoint(1));

    m_currentAngle = MathTools::angleInDegrees(vd1, vd2);
    m_currentAngle = (m_currentAngle > 180.0) ? m_currentAngle * -1 : m_currentAngle;
}

void AngleToolRepresentation::placeText()
{
    double *p1 = m_polyline->getPoint(0);
    double *p2 = m_polyline->getPoint(1);
    double *p3 = m_polyline->getPoint(2);
    double position[3];
    int i; 
    int horizontalCoord = Q2DViewer::getXIndexForView(m_view);
    int verticalCoord = Q2DViewer::getYIndexForView(m_view);

    //mirem on estan horitzontalment els punts p1 i p3 respecte del p2
    if (p1[0] <= p2[0])
    {
        m_text->setHorizontalJustification("Left");

        if (p3[horizontalCoord] <= p2[horizontalCoord])
        {
            m_text->setAttachmentPoint(p2);
        }
        else
        {
            for (i = 0; i < 3; i++)
            {
                position[i] = p2[i];
            }

            if (p2[verticalCoord] <= p3[verticalCoord])
            {
                position[verticalCoord] -= 2.;
            }
            else
            {
                position[verticalCoord] += 2.;
            }
            m_text->setAttachmentPoint(position);
        }
    }
    else
    {
        m_text->setHorizontalJustification("Right");

        if (p3[horizontalCoord] <= p2[horizontalCoord])
        {
            m_text->setAttachmentPoint(p2);
        }
        else
        {
            for (i = 0; i < 3; i++)
            {
                position[i] = p2[i];
            }

            if (p2[verticalCoord] <= p3[verticalCoord])
            {
                position[verticalCoord] += 2.;
            }
            else
            {
                position[verticalCoord] -= 2.;
            }
            m_text->setAttachmentPoint(position);
        }
    }
}

void AngleToolRepresentation::createHandlers()
{
    // HACK
    int zCoordinate = Q2DViewer::getZIndexForView(m_view);
    double depth = m_polyline->getPoint(0)[zCoordinate];

    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    for (int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        ToolHandlerWithRepresentation *handler = new ToolHandlerWithRepresentation(m_drawer, m_polyline->getPoint(i));
        m_toolHandlers << handler;
        m_toolHandlersMap.insert(i, handler);
        connect(handler, SIGNAL(movePoint(ToolHandler *, double *)), this, SLOT(movePoint(ToolHandler *, double *)));
        connect(handler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));
        connect(this, SIGNAL(hideToolHandlers()), handler, SLOT(hide()));
        connect(this, SIGNAL(showToolHandlers()), handler, SLOT(show()));

        connect(toolHandler, SIGNAL(moveAllPoints(double *)), handler, SLOT(move(double *)));

        handler->setParams(zCoordinate, depth);
    }

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(selectRepresentation()), this, SLOT(selectRepresentation()));

    toolHandler->setParams(zCoordinate, depth);

    m_toolHandlers << toolHandler;
}

QList<double *> AngleToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    const int key = m_toolHandlersMap.key(toolHandler);

    QList<double *> list;
    list << m_polyline->getPoint(key);

    // Tool edited
    m_edited = true;

    return list;
}

void AngleToolRepresentation::moveAllPoints(double *movement)
{
    double *point;

    for (int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        point = m_polyline->getPoint(i);

        point[0] += movement[0];
        point[1] += movement[1];
        point[2] += movement[2];
    }

    point = m_text->getAttachmentPoint();

    point[0] += movement[0];
    point[1] += movement[1];
    point[2] += movement[2];

    this->refresh();
}

}
