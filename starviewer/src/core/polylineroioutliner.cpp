/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineroioutliner.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawerpolygon.h"

// Vtk's
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

PolylineROIOutliner::PolylineROIOutliner(Q2DViewer *viewer, QObject *parent)
 : Outliner(viewer, parent), m_roiPolygon(0)
{
    m_closingPolyline = NULL;
    m_mainPolyline = NULL;

    // TODO Tenir en compte interrupcions de l'outliner, com canvi de volum 
    // o eliminació de l'outliner degut a events extern mentres estem editant la forma
}

PolylineROIOutliner::~PolylineROIOutliner()
{
    // TODO Tenir en compte si la forma encara s'està editant o no?
}

void PolylineROIOutliner::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateClosingPolyline();
            break;
    }
}

void PolylineROIOutliner::handlePointAddition()
{
    if (m_2DViewer->getInput())
    {
        switch (m_2DViewer->getInteractor()->GetRepeatCount())
        {
            case 0: // Single-click o primer click d'un doble click. Afegim un nou punt a la ROI
                annotateNewPoint();
                break;

            case 1: // Doble-click, si tenim més de 2 punts, llavors tanquem la ROI
                if (m_mainPolyline->getNumberOfPoints() > 2)
                {
                    closeForm();
                }
                break;
        }
    }
}

void PolylineROIOutliner::annotateNewPoint()
{
    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);
    
    bool firstPoint = false;
    if (!m_mainPolyline)
    {
        firstPoint = true;
        m_mainPolyline = new DrawerPolyline;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_mainPolyline->increaseReferenceCount();
    }
    // Afegim el punt de la nova polilínia
    m_mainPolyline->addPoint(pickedPoint);
    
    if (firstPoint) // L'afegim a l'escena
    {        
        m_2DViewer->getDrawer()->draw(m_mainPolyline);
    }
    else // Actualitzem l'estructura interna
    {
        m_mainPolyline->update();
    }

    if (!m_roiPolygon)
    {
        m_roiPolygon = new DrawerPolygon;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_roiPolygon->increaseReferenceCount();
    }
    // Afegim el punt al polígon de la ROI
    m_roiPolygon->addVertix(pickedPoint);
}

void PolylineROIOutliner::simulateClosingPolyline()
{
    if (m_mainPolyline && (m_mainPolyline->getNumberOfPoints() >= 1))
    {
        double pickedPoint[3];
        m_2DViewer->getEventWorldCoordinate(pickedPoint);
        m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);

        if (!m_closingPolyline)
        {
            m_closingPolyline = new DrawerPolyline;
            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_closingPolyline->increaseReferenceCount();
            m_closingPolyline->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);
            m_2DViewer->getDrawer()->draw(m_closingPolyline);

            // Afegim els punts que simulen aquesta polilínia
            m_closingPolyline->addPoint(m_mainPolyline->getPoint(0));
            m_closingPolyline->addPoint(pickedPoint);
            m_closingPolyline->addPoint(m_mainPolyline->getPoint(m_mainPolyline->getNumberOfPoints() - 1));
        }
        else
        {
            // Modifiquem els punts que han canviat
            m_closingPolyline->setPoint(1, pickedPoint);
            m_closingPolyline->setPoint(2, m_mainPolyline->getPoint(m_mainPolyline->getNumberOfPoints() - 1));
            // Actualitzem els atributs de la polilínia
            m_closingPolyline->update();
        }
        // TODO Fer servir Drawer::updateRenderer()?
        m_2DViewer->render();
    }
}

void PolylineROIOutliner::closeForm()
{
    // Així alliberem les primitives perquè puguin ser esborrades
    m_closingPolyline->decreaseReferenceCount();
    m_mainPolyline->decreaseReferenceCount();
    m_roiPolygon->decreaseReferenceCount();
    // Eliminem les polilínies amb les que hem simulat el dibuix de la ROI
    delete m_closingPolyline;
    delete m_mainPolyline;

    // Dibuixem el polígon resultant
    m_2DViewer->getDrawer()->draw(m_roiPolygon);

    // TODO Cal?
    // m_2DViewer->getDrawer()->updateRenderer();
    
    emit finished(m_roiPolygon);
    // Un cop fets els càlculs, fem el punter nul per poder controlar si podem fer una nova roi o no
    // No fem delete, perquè sinó això faria que s'esborrés del drawer
    // TODO De moment això ho deixem comentat, caldria mirar com fer la gestió d'interrupcions de les tools i de formes inacabades
//    m_roiPolygon = NULL;
}

}
