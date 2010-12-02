/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineangleoutliner.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "mathtools.h"

// VTK
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

PolylineAngleOutliner::PolylineAngleOutliner(Q2DViewer *viewer, QObject *parent)
 : Outliner(viewer, parent), m_mainPolyline(0), m_circlePolyline(0), m_state(None)
{
}

PolylineAngleOutliner::~PolylineAngleOutliner()
{
}

double PolylineAngleOutliner::getAngleDegrees()
{
    return m_currentAngle;
}

void PolylineAngleOutliner::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateCorrespondingSegmentOfAngle();
            break;
    }
}

void PolylineAngleOutliner::findInitialDegreeArc()
{
    // Per saber quin l'angle inicial, cal calcular l'angle que forma el primer segment anotat i un segment fictici totalment horitzontal.
    int horizontalCoordinate, zCoordinate;

    switch (m_2DViewer->getView())
    {
        case QViewer::AxialPlane:
            horizontalCoordinate = 0;
            zCoordinate = 2;
            break;

        case QViewer::SagitalPlane:
            horizontalCoordinate = 1;
            zCoordinate = 0;
            break;

        case QViewer::CoronalPlane:
            horizontalCoordinate = 2;
            zCoordinate = 1;
            break;
    }

    double horizontalPoint[3];
    double *secondPointOfAngle = m_mainPolyline->getPoint(1);
    
    for (int i = 0; i < 3; i++)
    {
        horizontalPoint[i] = secondPointOfAngle[i];
    }

    double *firstPointOfAngle = m_mainPolyline->getPoint(0);
    double *directorVector1 = MathTools::directorVector(firstPointOfAngle, secondPointOfAngle);

    horizontalPoint[horizontalCoordinate] += 10.0;
    double *directorVector2 = MathTools::directorVector(horizontalPoint, secondPointOfAngle);

    double crossProduct[3];
    MathTools::crossProduct(directorVector1, directorVector2, crossProduct);

    if (crossProduct[zCoordinate] > 0)
    {
        m_initialDegreeArc =(int)MathTools::angleInDegrees(directorVector1, directorVector2);
    }
    else
    {
        m_initialDegreeArc = -1 * (int)MathTools::angleInDegrees(directorVector1, directorVector2);
    }
}

void PolylineAngleOutliner::annotateFirstPoint()
{
    m_mainPolyline = new DrawerPolyline;

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(clickedWorldPoint);

    // Afegim el punt
    m_mainPolyline->addPoint(clickedWorldPoint);
    m_2DViewer->getDrawer()->drawWorkInProgress(m_mainPolyline);

    // Actualitzem l'estat de la tool
    m_state = FirstPointFixed;
}

void PolylineAngleOutliner::fixFirstSegment()
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(clickedWorldPoint);
    
	// Afegim el segon punt
    m_mainPolyline->addPoint(clickedWorldPoint);
    m_mainPolyline->update();

    // Posem l'estat de la tool a CenterFixed, així haurà agafat l'últim valor.
    m_state = CenterFixed;

    // Creem la polilínia per a dibuixar l'arc de circumferència i l'afegim al drawer
    m_circlePolyline = new DrawerPolyline;
    m_2DViewer->getDrawer()->drawWorkInProgress(m_circlePolyline);
}

void PolylineAngleOutliner::drawCircle()
{
    double *firstPoint = m_mainPolyline->getPoint(0);
    double *circleCentre = m_mainPolyline->getPoint(1);
    double *lastPoint = m_mainPolyline->getPoint(2);

    // Calculem l'angle que formen els dos segments
    double *firstSegment = MathTools::directorVector(firstPoint, circleCentre);
    double *secondSegment = MathTools::directorVector(lastPoint, circleCentre);
    m_currentAngle = MathTools::angleInDegrees(firstSegment, secondSegment);
    
    // Calculem el radi de l'arc de circumferència que mesurarà
    // un quart del segment més curt dels dos que formen l'angle
    double distance1 = MathTools::getDistance3D(firstPoint, circleCentre);
    double distance2 = MathTools::getDistance3D(circleCentre, lastPoint);    
    double radius = MathTools::minimum(distance1, distance2) / 4.0;

    // Calculem el rang de les iteracions per pintar l'angle correctament
    int initialAngle = 360 - m_initialDegreeArc;
    int finalAngle = int(360 - (m_currentAngle + m_initialDegreeArc));

    double crossProduct[3];
    MathTools::crossProduct(firstSegment, secondSegment,crossProduct);

    int view = m_2DViewer->getView();
    int zIndex = Q2DViewer::getZIndexForView(view);
    if (crossProduct[zIndex] > 0)
    {
        finalAngle = int(m_currentAngle - m_initialDegreeArc);
    }
    if ((initialAngle - finalAngle) > 180)
    {
        initialAngle = int(m_currentAngle - m_initialDegreeArc);
        finalAngle = -m_initialDegreeArc;
    }

    double degreesIncrease; 
    // Reconstruim l'arc de circumferència
    m_circlePolyline->deleteAllPoints();
    for (int i = initialAngle; i > finalAngle; --i)
    {
        degreesIncrease = i * 1.0 * MathTools::DegreesToRadiansAsDouble;
        double *newPoint = new double[3];

        // TODO Aquí hauríem de fer alguna cosa d'aquest estil, però si ho fem així, 
        // no se'ns dibuixa l'arc de circumferència que ens esperem sobre la vista coronal.
        // Potser és degut a com obtenim els punts o per una altra causa. Caldria mirar-ho 
        // per això evitar la consciència del pla en el que ens trobem
        // newPoint[xIndex] = cos(degreesIncrease) * radius + circleCentre[xIndex];
        // newPoint[yIndex] = sin(degreesIncrease) * radius + circleCentre[yIndex];
        // newPoint[zIndex] = 0.0;
        switch (view)
        {
            case QViewer::AxialPlane:
                newPoint[0] = cos(degreesIncrease) * radius + circleCentre[0];
                newPoint[1] = sin(degreesIncrease) * radius + circleCentre[1];
                newPoint[2] = 0.0;
                break;

            case QViewer::SagitalPlane:
                newPoint[0] = 0.0;
                newPoint[1] = cos(degreesIncrease) * radius + circleCentre[1];
                newPoint[2] = sin(degreesIncrease) * radius + circleCentre[2];
                break; 

            case QViewer::CoronalPlane:
                newPoint[0] = sin(degreesIncrease) * radius + circleCentre[0];
                newPoint[1] = 0.0;
                newPoint[2] = cos(degreesIncrease) * radius + circleCentre[2];
                break;
        }
        m_circlePolyline->addPoint(newPoint);
    }

    m_circlePolyline->update();
}

void PolylineAngleOutliner::handlePointAddition()
{
    if (m_2DViewer->getInput())
    {
        if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
        {
            if (m_state == None)
            {
                this->annotateFirstPoint();
                // TODO És necessari fer aquest update?
                m_2DViewer->getDrawer()->updateRenderer();
            }
            else if (m_state == FirstPointFixed)
            {
                this->fixFirstSegment();
                this->findInitialDegreeArc();
                // TODO És necessari fer aquest update?
                m_2DViewer->getDrawer()->updateRenderer();
            }
            else
            {
                // Voldrem enregistrar l'últim punt, pertant posem l'estat a none
                m_state = None;
                finishDrawing();
                //m_2DViewer->getDrawer()->updateRenderer();
            }
        }
    }
}

void PolylineAngleOutliner::simulateCorrespondingSegmentOfAngle()
{
    if (!m_mainPolyline)
    {
        return;
    }

    if (m_state != None)
    {
        // Agafem la coordenada de pantalla
        double clickedWorldPoint[3];
        m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

        int pointIndex;
        if (m_state == FirstPointFixed)
        {
            pointIndex = 1;
        }
        else if (m_state == CenterFixed)
        {
            pointIndex = 2;
        }

        // Assignem el segon o tercer punt de l'angle segons l'estat
        m_mainPolyline->setPoint(pointIndex, clickedWorldPoint);
        m_mainPolyline->update();

        if (m_state == CenterFixed)
        {
            drawCircle();
        }
        
        m_2DViewer->render();
    }
}

void PolylineAngleOutliner::finishDrawing()
{
    // Eliminem l'arc de circumferència (s'esborra automàticament del drawer)
    delete m_circlePolyline;
    m_circlePolyline = 0;

    emit finished(m_mainPolyline);
}

}
