/***************************************************************************
 *   Copyright (C) 2010 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "linepathtool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawerpolygon.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

LinePathTool::LinePathTool( QViewer *viewer, QObject *parent )
:Tool(viewer,parent)
{
    m_toolName = "LinePathTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );
    }

    connect( m_2DViewer, SIGNAL( volumeChanged(Volume *) ), SLOT( initialize() ) );
}

LinePathTool::~LinePathTool(void)
{
     bool hasToRefresh = false;
    // Cal decrementar el reference count perquè 
    // l'annotació s'esborri si "matem" l'eina
    if ( !m_polyline.isNull() )
    {
        m_polyline->decreaseReferenceCount();
        delete m_polyline;
        hasToRefresh = true;
    }

    if( hasToRefresh )
        m_2DViewer->render();
}

void LinePathTool::initialize()
{
    // Alliberem la primitiva perquè pugui ser esborrada
    if ( !m_polyline.isNull() )
    {
        m_polyline->decreaseReferenceCount();
        delete m_polyline;
    }

    m_polyline = NULL;
}

void LinePathTool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
        break;
    }
}

void LinePathTool::handlePointAddition()
{
    if( m_2DViewer->getInput() )
    {
        switch( m_2DViewer->getInteractor()->GetRepeatCount() )
        {
        case 0: // Single-click o primer click d'un doble click. Afegim un nou punt al path
            annotateNewPoint();
            break;

        case 1: // Doble-click, si tenim com a mínim dos punts llavors donem per finalitzat el path indicat
            if( m_polyline->getNumberOfPoints() >= 2 )
                closeForm();
            break;
        }
    }
}

void LinePathTool::annotateNewPoint()
{
    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);

    bool firstPoint = false;
    if (!m_polyline )
    {
        firstPoint = true;
        m_polyline = new DrawerPolyline;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_polyline->increaseReferenceCount();
    }
    
    // Afegim el punt de la polilínia que estem pintant
    m_polyline->addPoint( pickedPoint );

    if( firstPoint ) // L'afegim a l'escena
        m_2DViewer->getDrawer()->draw( m_polyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    else // Actualitzem l'estructura interna
    {
        m_polyline->update();
        m_2DViewer->render();
    }
}

void LinePathTool::closeForm()
{
    // Així alliberem la primitiva perquè pugui ser esborrada
    m_polyline->decreaseReferenceCount();
    
    // Indiquem que hem finalitzat les tasques de dibuix
    emit finished( m_polyline );

    // Eliminem la polilínia
    delete m_polyline;
}

}  //  end namespace udg
