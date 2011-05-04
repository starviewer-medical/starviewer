#include "ovalroitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "series.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "mathtools.h"

#include <vtkCommand.h>

namespace udg {

OvalROITool::OvalROITool(QViewer *viewer, QObject *parent)
 : ROITool(viewer, parent), m_state(Ready)
{
    m_toolName = "OvalROITool";

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume *)), SLOT(initialize()));
}

OvalROITool::~OvalROITool()
{
    if (!m_roiPolygon.isNull() && m_state == FirstPointFixed)
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = 0;
}

void OvalROITool::handleEvent(long unsigned eventID)
{
    if (!m_2DViewer->getInput())
    {
        return;
    }

    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateOval();
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            closeForm();
            break;
    }
}

void OvalROITool::handlePointAddition()
{
    if (m_state == Ready)
    {
        m_2DViewer->getEventWorldCoordinate(m_firstPoint);
        m_2DViewer->putCoordinateInCurrentImageBounds(m_firstPoint);

        memcpy(m_secondPoint, m_firstPoint, sizeof(double) * 3);

        m_state = FirstPointFixed;

        // Com que estem afegint punts cal indicar que és necessari recalcular les dades estadístiques
        m_hasToComputeStatisticsData = true;
    }
}

void OvalROITool::simulateOval()
{
    if (m_state == FirstPointFixed)
    {
        // Obtenim el segon punt
        m_2DViewer->getEventWorldCoordinate(m_secondPoint);
        m_2DViewer->putCoordinateInCurrentImageBounds(m_secondPoint);

        // Si encara no havíem creat el polígon, ho fem
        if (!m_roiPolygon)
        {
            m_roiPolygon = new DrawerPolygon;
            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_roiPolygon->increaseReferenceCount();
            m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        }

        // Actualitzem la forma i renderitzem
        updatePolygonPoints();
        m_2DViewer->render();
    }
}

void OvalROITool::computeOvalCentre(double centre[3])
{
    for (int i = 0; i < 3; ++i)
    {
        centre[i] = m_firstPoint[i] + (m_secondPoint[i] - m_firstPoint[i]) * 0.5;
    }
}

void OvalROITool::updatePolygonPoints()
{
    double centre[3];
    computeOvalCentre(centre);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    // Algorisme pel càlcul de l'el·lipse, extret de http://en.wikipedia.org/wiki/Ellipse#Ellipses_in_computer_graphics
    double xRadius = (m_secondPoint[xIndex] - m_firstPoint[xIndex]) * 0.5;
    double yRadius = (m_secondPoint[yIndex] - m_firstPoint[yIndex]) * 0.5;
    double depthValue = centre[zIndex];

    double beta = MathTools::degreesToRadians(360);
    double sinusBeta = sin(beta);
    double cosinusBeta = cos(beta);

    const int polygonPoints = 50;
    double alpha = 0.0;
    int vertixIndex = 0;
    for (double i = 0; i < 360.0; i += 360.0 / polygonPoints)
    {
        alpha = MathTools::degreesToRadians(i);
        double sinusAlpha = sin(alpha);
        double cosinusAlpha = cos(alpha);

        double polygonPoint[3];

        polygonPoint[xIndex] = centre[xIndex] + (xRadius * cosinusAlpha * cosinusBeta - yRadius * sinusAlpha * sinusBeta);
        polygonPoint[yIndex] = centre[yIndex] + (xRadius * cosinusAlpha * sinusBeta + yRadius * sinusAlpha * cosinusBeta);
        polygonPoint[zIndex] = depthValue;

        m_roiPolygon->setVertix(vertixIndex++, polygonPoint);
    }

    m_roiPolygon->update();
}

void OvalROITool::closeForm()
{
    // Cal comprovar si hi ha un objecte creat ja que podria ser que no s'hagués creat si s'hagués realitzat un doble clic,
    // per exemple, ja que no s'hauria passat per l'event de mouse move, que és quan es crea la primitiva.
    if (m_roiPolygon)
    {
        printData();
        // Alliberem la primitiva perquè pugui ser esborrada
        m_roiPolygon->decreaseReferenceCount();
        m_roiPolygon = 0;
    }

    m_state = Ready;
}

void OvalROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = 0;
    m_state = Ready;
}

}
