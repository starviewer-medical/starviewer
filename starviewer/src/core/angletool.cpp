#include "angletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "mathtools.h"
// Per la funció swap (si passem a C++11 s'haurà de canviar per <utility>)
#include <algorithm>
// VTK
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

#include <QVector2D>
#include <QVector3D>

namespace udg {

AngleTool::AngleTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_mainPolyline(NULL), m_circlePolyline(NULL), m_state(None)
{
    m_toolName = "AngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

AngleTool::~AngleTool()
{
    deleteTemporalRepresentation();
}

void AngleTool::deleteTemporalRepresentation()
{
    if (m_state != None)
    {
        bool hasToRefresh = false;
        // Cal decrementar el reference count perquè
        // l'annotació s'esborri si "matem" l'eina
        if (m_mainPolyline)
        {
            m_mainPolyline->decreaseReferenceCount();
            delete m_mainPolyline;
            hasToRefresh = true;
        }

        if (m_circlePolyline)
        {
            m_circlePolyline->decreaseReferenceCount();
            delete m_circlePolyline;
            hasToRefresh = true;
        }

        if (hasToRefresh)
        {
            m_2DViewer->render();
        }

        m_state = None;
    }
}

void AngleTool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateCorrespondingSegmentOfAngle();
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27) // ESC
            {
                deleteTemporalRepresentation();
            }
            break;
    }
}

void AngleTool::annotateFirstPoint()
{
    m_mainPolyline = new DrawerPolyline;

    // Obtenim el punt clickat
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    // Afegim el punt a la polilínia
    m_mainPolyline->addPoint(clickedWorldPoint);
    // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
    m_mainPolyline->increaseReferenceCount();
    m_2DViewer->getDrawer()->draw(m_mainPolyline);

    // Actualitzem l'estat de la tool
    m_state = FirstPointFixed;
}

void AngleTool::fixFirstSegment()
{
    // Posem l'estat de la tool a CenterFixed, així haurà agafat l'últim valor.
    m_state = CenterFixed;

    // Creem la polilínia per a dibuixar l'arc de circumferència i l'afegim al drawer
    m_circlePolyline = new DrawerPolyline;
    // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
    m_circlePolyline->increaseReferenceCount();
    m_2DViewer->getDrawer()->draw(m_circlePolyline);
}

void AngleTool::drawCircle()
{
    double *firstPoint = m_mainPolyline->getPoint(0);
    double *circleCentre = m_mainPolyline->getPoint(1);
    double *lastPoint = m_mainPolyline->getPoint(2);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
    QVector3D firstPointProjected(firstPoint[xIndex], firstPoint[yIndex], 0.0);
    QVector3D circleCentreProjected(circleCentre[xIndex], circleCentre[yIndex], 0.0);
    QVector3D lastPointProjected(lastPoint[xIndex], lastPoint[yIndex], 0.0);

    // Calculem l'angle que formen els dos segments
    QVector3D firstSegment = MathTools::directorVector(circleCentreProjected, firstPointProjected);
    QVector3D secondSegment = MathTools::directorVector(circleCentreProjected, lastPointProjected);
    m_currentAngle = MathTools::angleInDegrees(firstSegment, secondSegment);

    // Calculem el radi de l'arc de circumferència que mesurarà un quart del segment més curt dels dos que formen l'angle
    double distance1 = firstSegment.length();
    double distance2 = secondSegment.length();
    double radius = MathTools::minimum(distance1, distance2) / 4.0;

    // Calculem el rang de les iteracions per pintar l'angle correctament
    double initialAngle = MathTools::angleInRadians(firstSegment.toVector2D());
    if (initialAngle < 0.0)
    {
        initialAngle += 2.0 * MathTools::PiNumber;
    }
    double finalAngle = MathTools::angleInRadians(secondSegment.toVector2D());
    if (finalAngle < 0.0)
    {
        finalAngle += 2.0 * MathTools::PiNumber;
    }
    // Tenim els dos angles al rang [0,2pi)
    // Assegurem que girem en sentit horari -> tindrem angle positiu
    if (finalAngle < initialAngle)
    {
        std::swap(initialAngle, finalAngle);
    }
    double angle = finalAngle - initialAngle;
    // Assegurem que tenim l'angle al rang [0, pi]
    if (angle > MathTools::PiNumber)
    {
        angle = 2.0 * MathTools::PiNumber - angle;
        std::swap(initialAngle, finalAngle);
    }
    int degrees = qRound(angle * MathTools::RadiansToDegreesAsDouble);
    double increment = angle / degrees;

    // Reconstruim l'arc de circumferència
    m_circlePolyline->deleteAllPoints();
    for (int i = 0; i <= degrees; i++)
    {
        angle = initialAngle + i * increment;
        double newPoint[3];
        newPoint[xIndex] = cos(angle) * radius + circleCentre[xIndex];
        newPoint[yIndex] = sin(angle) * radius + circleCentre[yIndex];
        newPoint[zIndex] = 0.0;
        m_circlePolyline->addPoint(newPoint);
    }

    m_circlePolyline->update();
}

void AngleTool::handlePointAddition()
{
    if (m_2DViewer->getInput())
    {
        if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
        {
            if (m_state == None)
            {
                this->annotateFirstPoint();
            }
            else if (m_state == FirstPointFixed)
            {
                this->fixFirstSegment();
            }
            else
            {
                // Voldrem enregistrar l'últim punt, pertant posem l'estat a none
                m_state = None;
                finishDrawing();
            }
        }
    }
}

void AngleTool::simulateCorrespondingSegmentOfAngle()
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

void AngleTool::finishDrawing()
{
    // Així alliberem les primitives perquè puguin ser esborrades
    m_mainPolyline->decreaseReferenceCount();
    m_circlePolyline->decreaseReferenceCount();
    // Eliminem l'arc de circumferència (s'esborra automàticament del drawer)
    delete m_circlePolyline;

    // Col·loquem l'angle en el pla corresponent
    m_2DViewer->getDrawer()->erasePrimitive(m_mainPolyline);
    m_2DViewer->getDrawer()->draw(m_mainPolyline, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Afegim l'annotació textual
    DrawerText *text = new DrawerText;
    text->setText(tr("%1 degrees").arg(m_currentAngle, 0, 'f', 1));
    placeText(text);
    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void AngleTool::placeText(DrawerText *angleText)
{
    // Padding de 5 pixels
    const double Padding = 5.0;

    double *point1 = m_mainPolyline->getPoint(0);
    double *point2 = m_mainPolyline->getPoint(1);
    double *point3 = m_mainPolyline->getPoint(2);
    double position[3];
    Q2DViewer::CameraOrientationType view = m_2DViewer->getView();
    int xIndex = Q2DViewer::getXIndexForView(view);
    int yIndex = Q2DViewer::getYIndexForView(view);

    // Mirem on estan horitzontalment els punts point1 i point3 respecte del point2
    if (point1[0] <= point2[0])
    {
        angleText->setHorizontalJustification("Left");

        if (point3[xIndex] <= point2[xIndex])
        {
            angleText->setAttachmentPoint(point2);
        }
        else
        {
            double point2InDisplay[3];
            // Passem point2 a coordenades de display
            m_2DViewer->computeWorldToDisplay(point2[0], point2[1], point2[2], point2InDisplay);

            // Apliquem el padding
            if (point2[yIndex] <= point3[yIndex])
            {
                point2InDisplay[1] -= Padding;
            }
            else
            {
                point2InDisplay[1] += Padding;
            }
            // Tornem a coordenades de món
            double temporalWorldPoint[4];
            m_2DViewer->computeDisplayToWorld(point2InDisplay[0], point2InDisplay[1], point2InDisplay[2], temporalWorldPoint);
            position[0] = temporalWorldPoint[0];
            position[1] = temporalWorldPoint[1];
            position[2] = temporalWorldPoint[2];

            // Ara position és l'attachment point que volem
            angleText->setAttachmentPoint(position);
        }
    }
    else
    {
        angleText->setHorizontalJustification("Right");

        if (point3[xIndex] <= point2[xIndex])
        {
            angleText->setAttachmentPoint(point2);
        }
        else
        {
            double point2InDisplay[3];
            // Passem point2 a coordenades de display
            m_2DViewer->computeWorldToDisplay(point2[0], point2[1], point2[2], point2InDisplay);

            // Apliquem el padding
            if (point2[yIndex] <= point3[yIndex])
            {
                point2InDisplay[1] += Padding;
            }
            else
            {
                point2InDisplay[1] -= Padding;
            }
            // Tornem a coordenades de món
            double temporalWorldPoint[4];
            m_2DViewer->computeDisplayToWorld(point2InDisplay[0], point2InDisplay[1], point2InDisplay[2], temporalWorldPoint);
            position[0] = temporalWorldPoint[0];
            position[1] = temporalWorldPoint[1];
            position[2] = temporalWorldPoint[2];

            // Ara position és l'attachment point que volem
            angleText->setAttachmentPoint(position);
        }
    }
}

void AngleTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (m_mainPolyline)
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
    }

    if (m_circlePolyline)
    {
        m_circlePolyline->decreaseReferenceCount();
        delete m_circlePolyline;
    }

    m_mainPolyline = NULL;
    m_circlePolyline = NULL;
    m_state = None;
}

}
