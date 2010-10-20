/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extractimagetoolrepresentation.h"

#include "drawer.h"
#include "drawerpolyline.h"
#include "drawerimage.h"
#include "q2dviewer.h"
#include "mathtools.h"
#include "toolhandlerrotation.h"
#include "toolhandlerwithoutrepresentation.h"

//vtk
#include <vtkTransform.h>

namespace udg {

ExtractImageToolRepresentation::ExtractImageToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent)
{
}

ExtractImageToolRepresentation::~ExtractImageToolRepresentation()
{
    delete m_polyline;
    delete m_image;
}

void ExtractImageToolRepresentation::setPolyline(DrawerPolyline *polyline)
{
    m_polyline = polyline;
    m_primitivesList << m_polyline;
}

void ExtractImageToolRepresentation::setImageData(DrawerImage *image)
{
    m_image = image;
    m_primitivesList << m_image;
}

void ExtractImageToolRepresentation::setParams(int view, double *origin)
{
    m_view = view;
    m_origin = origin;
}

void ExtractImageToolRepresentation::calculate()
{
    if (m_toolHandlers.size() == 0) //already created
    {
        this->createHandlers();
    }
}

void ExtractImageToolRepresentation::createHandlers()
{
    // HACK
    int zCoordinate = Q2DViewer::getZIndexForView(m_view);
    double depth = m_polyline->getPoint(0)[zCoordinate];

    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    findPolygonCentroid();

    for (int i = 0; i < m_polyline->getNumberOfPoints() - 1; i++)
    {
        ToolHandlerRotation *handler = new ToolHandlerRotation(m_drawer, m_polyline->getPoint(i));
        m_toolHandlers << handler;
        m_toolHandlersMap.insert(i, handler);
        connect(handler, SIGNAL(rotate(double)), this, SLOT(rotateRepresentation(double)));
        connect(handler, SIGNAL(destroy()), this, SLOT(destroy()));
        connect(this, SIGNAL(hideToolHandlers()), handler, SLOT(hide()));
        connect(this, SIGNAL(showToolHandlers()), handler, SLOT(show()));

        connect(toolHandler, SIGNAL(moveAllPoints(double *)), handler, SLOT(move(double *)));

        handler->setParams(zCoordinate, depth);
        handler->setRotationCenter(m_centroid);
    }

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(destroy()), this, SLOT(destroy()));

    toolHandler->setParams(zCoordinate, depth);

    m_toolHandlers << toolHandler;
}

QList<double *> ExtractImageToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    const int key = m_toolHandlersMap.key(toolHandler);

    QList<double *> list;
    list << m_polyline->getPoint(key);

    if (key == 0) //first point
    {
        list << m_polyline->getPoint(m_polyline->getNumberOfPoints() - 1); //last point also
    }

    return list;
}

void ExtractImageToolRepresentation::findPolygonCentroid()
{
    int xCoordinate, yCoordinate, zCoordinate;
    Q2DViewer::getXYZIndexesForView(xCoordinate, yCoordinate, zCoordinate, m_view);

    m_centroid[0] = 0;
    m_centroid[1] = 0;
    m_centroid[2] = 0;

    foreach (double *vertex, m_polyline->getPointsList())
    {
        m_centroid[xCoordinate] += vertex[xCoordinate];
        m_centroid[yCoordinate] += vertex[yCoordinate];
        m_centroid[zCoordinate] += vertex[zCoordinate];
    }

    m_centroid[xCoordinate] /= m_polyline->getNumberOfPoints();
    m_centroid[yCoordinate] /= m_polyline->getNumberOfPoints();
    m_centroid[zCoordinate] /= m_polyline->getNumberOfPoints();
}

void ExtractImageToolRepresentation::moveAllPoints(double *movement)
{
    double *point;

    for (int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        point = m_polyline->getPoint(i);

        point[0] += movement[0];
        point[1] += movement[1];
        point[2] += movement[2];
    }

    findPolygonCentroid();

    for (int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        ToolHandler *handler = m_toolHandlersMap.value(i);
        ToolHandlerRotation *rotationHandler = qobject_cast<ToolHandlerRotation *>(handler);
        if (rotationHandler)
        {
            rotationHandler->setRotationCenter(m_centroid);
        }
    }

    m_image->move(movement);


    this->refresh();
}

void ExtractImageToolRepresentation::rotateRepresentation(double angle)
{
    /*Càlcul de la transformació*/
    int xCoordinate, yCoordinate, zCoordinate;
    Q2DViewer::getXYZIndexesForView(xCoordinate, yCoordinate, zCoordinate, m_view);

    double *dv1, *dv2, *cross;
    dv1 = MathTools::directorVector(m_polyline->getPoint(0), m_centroid);
    dv2 = MathTools::directorVector(m_polyline->getPoint(1), m_centroid);
    MathTools::crossProduct(dv1,dv2,cross);
    cross[zCoordinate] = -cross[zCoordinate];

    m_transform = vtkTransform::New();
    m_transform->Identity();
    m_transform->Translate((m_centroid[xCoordinate]), (m_centroid[yCoordinate]), (m_centroid[zCoordinate]));
    m_transform->RotateWXYZ(angle, cross);
    m_transform->Translate(-(m_centroid[xCoordinate]), -(m_centroid[yCoordinate]), -(m_centroid[zCoordinate]));

    /*Apliquem la transformació a tots els punts*/
    double *point;

    for (int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        point = m_polyline->getPoint(i);
        rotatePoint(point);
        ToolHandler *handler = m_toolHandlersMap.value(i);
        ToolHandlerRotation *rotationHandler = qobject_cast<ToolHandlerRotation *>(handler);
        if (rotationHandler)
        {
            rotationHandler->setPosition(point);
        }
    }

    cross[zCoordinate] = -cross[zCoordinate];
    m_image->rotate(m_transform);
    m_transform->Delete();

    findPolygonCentroid();
    for (int i = 0; i < m_polyline->getNumberOfPoints(); i++)
    {
        ToolHandler *handler = m_toolHandlersMap.value(i);
        ToolHandlerRotation *rotationHandler = qobject_cast<ToolHandlerRotation *>(handler);
        if (rotationHandler)
        {
            rotationHandler->setRotationCenter(m_centroid);
        }
    }


    this->refresh();
}

void ExtractImageToolRepresentation::rotatePoint(double *point)
{
    // Apliquem la transformacio
    double newPoint[3];
    m_transform->TransformPoint(point, newPoint);

    point[0] = newPoint[0];
    point[1] = newPoint[1];
    point[2] = newPoint[2];
}

}

