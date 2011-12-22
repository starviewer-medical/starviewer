#include "circletool.h"

#include "drawer.h"
#include "drawercrosshair.h"
#include "drawerpolygon.h"
#include "logging.h"
#include "mathtools.h"
#include "q2dviewer.h"

#include <cmath>

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

CircleTool::CircleTool(QViewer *viewer, QObject *parent)
    : Tool(viewer, parent)
{
    m_toolName = "CircleTool";
    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);

    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));

    initialize();
}

CircleTool::~CircleTool()
{
    initialize();
}

void CircleTool::handleEvent(unsigned long eventId)
{
    if (!m_2DViewer || !m_2DViewer->getInput())
    {
        return;
    }

    switch (eventId)
    {
        case vtkCommand::LeftButtonPressEvent:
            startDrawing();
            break;
        case vtkCommand::MouseMoveEvent:
            if (m_isDrawing)
            {
                updateCircle();
            }
            break;
        case vtkCommand::LeftButtonReleaseEvent:
            if (m_isDrawing)
            {
                endDrawing();
            }
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27 && m_isDrawing)  // Esc
            {
                abortDrawing();
            }
            break;
    }
}

void CircleTool::startDrawing()
{
    Q_ASSERT(!m_isDrawing);

    m_isDrawing = true;
    
    double startPoint[3];
    m_2DViewer->getEventWorldCoordinate(startPoint);
    m_startPoint.x = startPoint[0];
    m_startPoint.y = startPoint[1];
    m_startPoint.z = startPoint[2];
}

void CircleTool::endDrawing()
{
    Q_ASSERT(m_isDrawing);

    // Cal comprovar si hi ha un objecte creat ja que podria ser que no s'hagués creat si s'hagués realitzat un doble clic,
    // per exemple, ja que no s'hauria passat per l'event de mouse move, que és quan es crea la primitiva.
    if (m_circle)
    {
        // Alliberem la primitiva perquè pugui ser esborrada
        m_circle->decreaseReferenceCount();
        // Pintem la primitiva al lloc corresponent
        m_2DViewer->getDrawer()->erasePrimitive(m_circle);
        m_2DViewer->getDrawer()->draw(m_circle, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        // Inicialitzem el punter a 0
        m_circle = 0;

        // Dibuixem la creu al centre
        Vector3 center = getCenter();
        DrawerCrossHair *cross = new DrawerCrossHair();
        cross->setCentrePoint(center.x, center.y, center.z);
        m_2DViewer->getDrawer()->draw(cross, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    }

    m_isDrawing = false;
}

void CircleTool::abortDrawing()
{
    Q_ASSERT(m_isDrawing);

    initialize();
}

void CircleTool::updateCircle()
{
    Q_ASSERT(m_isDrawing);

    getEndPoint();

    // Si encara no havíem creat el polígon, ho fem
    if (!m_circle)
    {
        m_circle = new DrawerPolygon();
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_circle->increaseReferenceCount();
        m_2DViewer->getDrawer()->draw(m_circle);
    }

    // Actualitzem la forma i renderitzem
    updatePolygonPoints();
    m_2DViewer->render();
}

void CircleTool::getEndPoint()
{
    double endPoint[3];
    m_2DViewer->getEventWorldCoordinate(endPoint);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
    double startPoint[3] = { m_startPoint.x, m_startPoint.y, m_startPoint.z };

    double width = endPoint[xIndex] - startPoint[xIndex];
    double height = endPoint[yIndex] - startPoint[yIndex];
    
    // Ens quedem amb la mida més gran
    if (qAbs(width) > qAbs(height))
    {
        endPoint[yIndex] = startPoint[yIndex] + MathTools::copySign(width, height);
    }
    else
    {
        endPoint[xIndex] = startPoint[xIndex] + MathTools::copySign(height, width);
    }

    m_endPoint.x = endPoint[0];
    m_endPoint.y = endPoint[1];
    m_endPoint.z = endPoint[2];
}

void CircleTool::updatePolygonPoints()
{
    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    double startPoint[3] = { m_startPoint.x, m_startPoint.y, m_startPoint.z };
    Vector3 vCenter = getCenter();
    double center[3] = { vCenter.x, vCenter.y, vCenter.z };
    double a = center[xIndex];
    double b = center[yIndex];
    double radius = qAbs(startPoint[xIndex] - center[xIndex]);

    m_circle->removeVertices();

    const int NumberOfPoints = 360;
    
    for (int i = 0; i < NumberOfPoints; i++)
    {
        double angle = static_cast<double>(i) / NumberOfPoints * 2.0 * MathTools::PiNumber;
        double point[3];
        point[xIndex] = a + radius * cos(angle);
        point[yIndex] = b + radius * sin(angle);
        point[zIndex] = center[zIndex];
        m_circle->addVertix(point);
    }

    m_circle->update();
}

Vector3 CircleTool::getCenter() const
{
    return 0.5 * (m_startPoint + m_endPoint);
}

void CircleTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (m_circle)
    {
        m_circle->decreaseReferenceCount();
        delete m_circle;
        m_2DViewer->render();
    }

    m_circle = 0;
    m_isDrawing = false;
}

}