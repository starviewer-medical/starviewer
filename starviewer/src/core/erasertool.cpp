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
// vtk
#include <vtkCommand.h>

namespace udg {

EraserTool::EraserTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_state(None), m_polygon(0)
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
    // A l'agafar el primer punt inicialitzem l'start i l'end point per igual
    // simplement per què així és més segur que no tenir un valor arbitrari a endPoint
    m_endPoint[0] = m_startPoint[0];
    m_endPoint[1] = m_startPoint[1];
    m_endPoint[2] = m_startPoint[2];

    m_state = StartClick;
}

void EraserTool::drawAreaOfErasure()
{
    if (m_state == StartClick)
    {
        double p2[3], p3[3];
        int xIndex, yIndex, zIndex;

        m_2DViewer->getEventWorldCoordinate( m_endPoint );
        Q2DViewer::getXYZIndexesForView( xIndex, yIndex, zIndex, m_2DViewer->getView() );

        // Calculem el segon punt i el tercer
        p2[xIndex] = m_endPoint[xIndex];
        p2[yIndex] = m_startPoint[yIndex];
        p2[zIndex] = m_2DViewer->getCurrentSlice();

        p3[xIndex] = m_startPoint[xIndex];
        p3[yIndex] = m_endPoint[yIndex];
        p3[zIndex] = m_2DViewer->getCurrentSlice();

        if (!m_polygon)
        {
            m_polygon = new DrawerPolygon;
            m_polygon->addVertix(p2);
            m_polygon->addVertix(m_endPoint);
            m_polygon->addVertix(p3);
            m_polygon->addVertix(m_startPoint);
            m_2DViewer->getDrawer()->draw(m_polygon, Q2DViewer::Top2DPlane);
        }
        else
        {
            // Assignem els punts del polígon
            m_polygon->setVertix( 0, p2 );
            m_polygon->setVertix( 1, m_endPoint );
            m_polygon->setVertix( 2, p3 );
            m_polygon->setVertix( 3, m_startPoint );
            // Actualitzem els atributs de la polilinia
            m_polygon->update();
            m_2DViewer->render();
        }
    }
}

void EraserTool::erasePrimitive()
{
    if (!m_polygon)
    {
        DrawerPrimitive *primitiveToErase = m_2DViewer->getDrawer()->getPrimitiveNearerToPoint(m_startPoint, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        if (primitiveToErase)
        {
            m_2DViewer->getDrawer()->erasePrimitive(primitiveToErase);
            m_2DViewer->render();
        }
    }
    else
    {
        double bounds[6];
        m_polygon->getBounds(bounds);
        m_2DViewer->getDrawer()->erasePrimitivesInsideBounds( bounds, m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
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
