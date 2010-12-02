#include "ellipseoutliner.h"

#include "drawerpolygon.h"
#include "q2dviewer.h"
#include "drawer.h"
#include "mathtools.h"

#include <vtkCommand.h>

namespace udg {

EllipseOutliner::EllipseOutliner(Q2DViewer *viewer, QObject *parent)
 : Outliner(viewer, parent), m_state(Ready), m_ellipsePolygon(0)
{
    // TODO Tenir en compte interrupcions de l'outliner, com canvi de volum 
    // o eliminació de l'outliner degut a events extern mentres estem editant la forma
}

EllipseOutliner::~EllipseOutliner()
{
    // TODO Tenir en compte si la forma encara s'està editant o no?
}

void EllipseOutliner::handleEvent(long unsigned eventID)
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
            simulateEllipse();
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            closeForm();
            break;
    }
}

void EllipseOutliner::handlePointAddition()
{
    if (m_state == Ready)
    {
        m_2DViewer->getEventWorldCoordinate(m_firstPoint);
        m_2DViewer->putCoordinateInCurrentImageBounds(m_firstPoint);
        
        memcpy(m_secondPoint, m_firstPoint, sizeof(double) * 3);

        m_state = FirstPointFixed;
    }
}

void EllipseOutliner::simulateEllipse()
{
    if (m_state == FirstPointFixed)
    {
        // Obtenim el segon punt
        m_2DViewer->getEventWorldCoordinate(m_secondPoint);
        m_2DViewer->putCoordinateInCurrentImageBounds(m_secondPoint);
        
        // Si encara no havíem creat el polígon, ho fem
        if (!m_ellipsePolygon)
        {            
            m_ellipsePolygon = new DrawerPolygon;
            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_ellipsePolygon->increaseReferenceCount();
            m_2DViewer->getDrawer()->drawWorkInProgress(m_ellipsePolygon);
        }

        // Actualitzem la forma i renderitzem
        updatePolygonPoints();
        // TODO Fer servir Drawer::updateRenderer()?
        m_2DViewer->render();    
    }
}

void EllipseOutliner::computeEllipseCentre(double centre[3])
{
    for (int i = 0; i < 3; ++i)
    {
        centre[i] = m_firstPoint[i] + (m_secondPoint[i] - m_firstPoint[i]) * 0.5;
    }
}

void EllipseOutliner::updatePolygonPoints()
{
    double centre[3];
    computeEllipseCentre(centre);

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
    for (double i = 0; i < 360.0; i += 360.0 / polygonPoints )
    {
        alpha = MathTools::degreesToRadians(i);
        double sinusAlpha = sin(alpha);
        double cosinusAlpha = cos(alpha);

        double polygonPoint[3];

        polygonPoint[xIndex] = centre[xIndex] + (xRadius * cosinusAlpha * cosinusBeta - yRadius * sinusAlpha * sinusBeta);
        polygonPoint[yIndex] = centre[yIndex] + (xRadius * cosinusAlpha * sinusBeta + yRadius * sinusAlpha * cosinusBeta);
        polygonPoint[zIndex] = depthValue;

        m_ellipsePolygon->setVertix(vertixIndex++, polygonPoint);
    }

    m_ellipsePolygon->update();
}

void EllipseOutliner::closeForm()
{
    // Cal comprovar si hi ha un objecte creat ja que podria ser que no s'hagués creat si s'hagués realitzat un doble clic, 
    // per exemple, ja que no s'hauria passat per l'event de mouse move, que és quan es crea la primitiva.
    if (m_ellipsePolygon)
    {
        // Alliberem la primitiva perquè pugui ser esborrada
        m_ellipsePolygon->decreaseReferenceCount();
        emit finished(m_ellipsePolygon);
        // TODO De moment això ho deixem comentat, caldria mirar com fer la gestió d'interrupcions de les tools i de formes inacabades
        //m_ellipsePolygon = 0;
    }
    m_state = Ready;    
}

}
