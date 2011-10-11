#include "nonclosedangletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"
#include "mathtools.h"
// VTK
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

#include <QVector3D>

namespace udg {

NonClosedAngleTool::NonClosedAngleTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_firstLine(0), m_secondLine(0), m_state(None), m_lineState(NoPoints)
{
    m_toolName = "NonClosedAngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

NonClosedAngleTool::~NonClosedAngleTool()
{
    deleteTemporalRepresentation();
}

void NonClosedAngleTool::deleteTemporalRepresentation()
{
    bool hasToRefresh = false;
    // Cal decrementar el reference count perquè
    // l'annotació s'esborri si "matem" l'eina
    if (m_firstLine)
    {
        m_firstLine->decreaseReferenceCount();
        delete m_firstLine;
        hasToRefresh = true;
    }

    if (m_secondLine)
    {
        m_secondLine->decreaseReferenceCount();
        delete m_secondLine;
        hasToRefresh = true;
    }

    if (hasToRefresh)
    {
        m_2DViewer->render();
    }

    m_state = None;
    m_lineState = NoPoints;
}

void NonClosedAngleTool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            handleLineDrawing();
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

void NonClosedAngleTool::handlePointAddition()
{
    if (m_2DViewer->getInput())
    {
        if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
        {
            this->annotateLinePoints();

            if (m_state == SecondLineFixed)
            {
                computeAngle();
                // Així alliberem les primitives perquè puguin ser esborrades
                m_firstLine->decreaseReferenceCount();
                m_secondLine->decreaseReferenceCount();
                m_middleLine->decreaseReferenceCount();
                // Acabem les línies
                m_firstLine = NULL;
                m_secondLine = NULL;
                m_middleLine = NULL;

                // Restaurem m_state
                m_state = None;
            }
        }
    }
}

void NonClosedAngleTool::annotateLinePoints()
{
    DrawerLine *line;

    // Creem primera o segona línies
    if ((m_state == None && m_lineState == NoPoints) || (m_state == FirstLineFixed && m_lineState == NoPoints))
    {
        line = new DrawerLine;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        line->increaseReferenceCount();
    }
    else if (m_state == None)
    {
        line = m_firstLine;
    }
    else
    {
        line = m_secondLine;
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    // Afegim el punt
    if (m_lineState == NoPoints)
    {
        line->setFirstPoint(clickedWorldPoint);
        line->setSecondPoint(clickedWorldPoint);
        m_lineState = FirstPoint;

        if (m_state == None)
        {
            m_firstLine = line;
        }
        else
        {
            m_secondLine = line;
        }

        m_2DViewer->getDrawer()->draw(line, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    }
    else
    {
        line->setSecondPoint(clickedWorldPoint);
        line->update();

        m_lineState = NoPoints;

        if (m_state == None)
        {
            m_state = FirstLineFixed;
        }
        else
        {
            m_state = SecondLineFixed;
        }
    }
}

void NonClosedAngleTool::handleLineDrawing()
{
    if (m_firstLine && m_state == None)
    {
        this->simulateLine(m_firstLine);
    }
    else if (m_secondLine && m_state == FirstLineFixed)
    {
        this->simulateLine(m_secondLine);
    }
}

void NonClosedAngleTool::simulateLine(DrawerLine *line)
{
    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    line->setSecondPoint(clickedWorldPoint);
    // Actualitzem viewer
    line->update();
    m_2DViewer->render();
}

void NonClosedAngleTool::computeAngle()
{
    if (!m_middleLine)
    {
        m_middleLine = new DrawerLine;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_middleLine->increaseReferenceCount();
    }
    m_middleLine->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);

    double *point1 = m_firstLine->getFirstPoint();
    double *point2 = m_firstLine->getSecondPoint();
    double *point3 = m_secondLine->getFirstPoint();
    double *point4 = m_secondLine->getSecondPoint();

    int state;

    double *intersection = MathTools::infiniteLinesIntersection(point1, point2, point3, point4, state);

    double distance1 = MathTools::getDistance3D(intersection, point1);
    double distance2 = MathTools::getDistance3D(intersection, point2);
    double distance3 = MathTools::getDistance3D(intersection, point3);
    double distance4 = MathTools::getDistance3D(intersection, point4);

    double *directorVector1;
    double *directorVector2;
    // Per calcular el vectors directors farem servir la intersecció i el punt
    // més llunyà a la intersecció de cada recta ja que si per alguna casualitat
    // l'usuari fa coincidir un dels punts de cada recta, la distància seria de 0
    // i com a conseqüència l'angle calculat ens sortiria Nan
    if (distance1 <= distance2)
    {
        if (distance3 <= distance4)
        {
            directorVector1 = MathTools::directorVector(point2, intersection);
            directorVector2 = MathTools::directorVector(point4, intersection);
            m_middleLine->setFirstPoint(point1);
            m_middleLine->setSecondPoint(point3);
        }
        else
        {
            directorVector1 = MathTools::directorVector(point2, intersection);
            directorVector2 = MathTools::directorVector(point3, intersection);
            m_middleLine->setFirstPoint(point1);
            m_middleLine->setSecondPoint(point4);
        }
    }
    else
    {
        if (distance3 <= distance4)
        {
            directorVector1 = MathTools::directorVector(point1, intersection);
            directorVector2 = MathTools::directorVector(point4, intersection);
            m_middleLine->setFirstPoint(point2);
            m_middleLine->setSecondPoint(point3);
        }
        else
        {
            directorVector1 = MathTools::directorVector(point1, intersection);
            directorVector2 = MathTools::directorVector(point3, intersection);
            m_middleLine->setFirstPoint(point2);
            m_middleLine->setSecondPoint(point4);
        }
    }

    // Dibuixem la línia auxiliar
    m_2DViewer->getDrawer()->draw(m_middleLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    for (int i = 0; i < 3; i++)
    {
        if (fabs(directorVector1[i]) < 0.0001)
        {
            directorVector1[i] = 0.0;
        }

        if (fabs(directorVector2[i]) < 0.0001)
        {
            directorVector2[i] = 0.0;
        }
    }

    double angle = MathTools::angleInDegrees(QVector3D(directorVector1[0], directorVector1[1], directorVector1[2]), 
        QVector3D(directorVector2[0], directorVector2[1], directorVector2[2]));

    DrawerText *text = new DrawerText;

    if (state == MathTools::ParallelLines)
    {
        text->setText(tr("0.0 degrees"));
    }
    // Won't occur
    else if (state == MathTools::SkewIntersection)
    {
        text->setText(tr("Skew lines."));
    }
    else
    {
        text->setText(tr("%1 degrees").arg(angle, 0, 'f', 1));
    }

    placeText(m_middleLine->getFirstPoint(), m_middleLine->getSecondPoint(), text);
    text->shadowOn();
    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void NonClosedAngleTool::placeText(double *firstLineVertex, double *secondLineVertex, DrawerText *angleText)
{
    double position[3];
    int xIndex, yIndex, zIndex;

    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
    position[xIndex] = (firstLineVertex[xIndex] + secondLineVertex[xIndex]) / 2.0;
    position[yIndex] = (firstLineVertex[yIndex] + secondLineVertex[yIndex]) / 2.0;
    position[zIndex] = firstLineVertex[zIndex];

    angleText->setAttachmentPoint(position);
}

void NonClosedAngleTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (m_firstLine)
    {
        m_firstLine->decreaseReferenceCount();
        delete m_firstLine;
    }

    if (m_secondLine)
    {
        m_secondLine->decreaseReferenceCount();
        delete m_secondLine;
    }

    m_firstLine = NULL;
    m_secondLine = NULL;
    m_state = None;
    m_lineState = NoPoints;
}

}
