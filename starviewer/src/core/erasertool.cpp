/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "erasertool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "mathtools.h"
// vtk
#include <vtkCommand.h>

namespace udg {

EraserTool::EraserTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_state(0), m_polygon(0)
{
    m_toolName = "EraserTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *)), SLOT(reset()) );
}

EraserTool::~EraserTool()
{
    if ( m_polygon )
        delete m_polygon;
}

void EraserTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            startEraserAction();
        break;

        case vtkCommand::MouseMoveEvent:
            if ( m_polygon && m_state == StartClick )
                drawAreaOfErasure();
        break;

        case vtkCommand::LeftButtonReleaseEvent:
            erasePrimitive();
            reset();
        break;

        default:
        break;
    }
}

void EraserTool::startEraserAction()
{
    m_2DViewer->getEventWorldCoordinate( m_startPoint );
    /// La següent inicialització de l'm_endPoint és per la distància que es calcula al mètode erasePrimitive(). 
    /// El primer cop que es calcula, aquest punt no tindrà valor i, per tant, ens donaria error
    m_endPoint[0] = m_startPoint[0];
    m_endPoint[1] = m_startPoint[1];
    m_endPoint[2] = m_startPoint[2];

    if (!m_polygon )
    {
        m_polygon = new DrawerPolygon;
        m_polygon->addVertix( m_startPoint );
        m_polygon->addVertix( m_startPoint );
        m_polygon->addVertix( m_startPoint );
        m_polygon->addVertix( m_startPoint );
        m_2DViewer->getDrawer()->draw( m_polygon , Q2DViewer::Top2DPlane );
    }

    m_state = StartClick;
}

void EraserTool::drawAreaOfErasure()
{
    double p2[3], p3[3];
    
    m_2DViewer->getEventWorldCoordinate( m_endPoint );

    // Calculem el segon punt i el tercer
    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::AxialPlane:
            p2[0] = m_endPoint[0];
            p2[1] = m_startPoint[1];
            p2[2] = m_2DViewer->getCurrentSlice();

            p3[0] = m_startPoint[0];
            p3[1] = m_endPoint[1];
            p3[2] = m_2DViewer->getCurrentSlice();
            break;
        case Q2DViewer::SagitalPlane:
            p2[0] = m_2DViewer->getCurrentSlice();
            p2[1] = m_startPoint[1];
            p2[2] = m_endPoint[2];

            p3[0] = m_2DViewer->getCurrentSlice();
            p3[1] = m_endPoint[1];
            p3[2] = m_startPoint[2];
            break;
        case Q2DViewer::CoronalPlane:
            p2[0] = m_startPoint[0];
            p2[1] = m_2DViewer->getCurrentSlice();
            p2[2] = m_endPoint[2];

            p3[0] = m_endPoint[0];
            p3[1] = m_2DViewer->getCurrentSlice();
            p3[2] = m_startPoint[2];
            break;
    }
    m_polygon->setVertix( 0, p2 );
    m_polygon->setVertix( 1, m_endPoint );
    m_polygon->setVertix( 2, p3 );
    m_polygon->setVertix( 3, m_startPoint );
    // Actualitzem els atributs de la polilinia
    m_polygon->update();

    m_2DViewer->render();
}

void EraserTool::erasePrimitive()
{
    if ( MathTools::getDistance3D( m_startPoint, m_endPoint ) <= 5.0 )
    {
        m_2DViewer->getDrawer()->erasePrimitive( m_2DViewer->getDrawer()->getPrimitiveNearerToPoint( m_startPoint, m_2DViewer->getView(), m_2DViewer->getCurrentSlice() ) );
    }
    else
        m_2DViewer->getDrawer()->erasePrimitivesInsideBounds( m_startPoint, m_endPoint, m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
}

void EraserTool::reset()
{
    if ( m_polygon )
    {
        delete m_polygon;
        m_polygon = NULL;
    }

    m_state = None;
}

}
