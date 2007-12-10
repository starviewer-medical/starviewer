/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineroitool.h"
#include "polylineroitooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "series.h"
#include "drawer.h"
#include "drawerpolyline.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
//Qt


namespace udg {

PolylineROITool::PolylineROITool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "PolylineROITool";
    m_hasSharedData = false;

//     m_toolData = new PolylineROIToolData;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    m_latestTime = 0;
    m_closingPolyline = NULL;
    m_mainPolyline=NULL;
}

PolylineROITool::~PolylineROITool()
{
}

void PolylineROITool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            this->annotateNewPoint();
            m_2DViewer->getDrawer()->refresh();

//             int totalTimeElapsed = m_time.elapsed();
//             int timeElapsed = ( totalTimeElapsed - m_latestTime );
// 
//             DEBUG_LOG( tr("CLIKS: %1").arg( m_2DViewer->getInteractor()->GetRepeatCount() ) );
// 
//             if( timeElapsed < 350 )
//             {
//                 DEBUG_LOG( "DOBLE CLICK");
//             }
//             else
//             {
//                 DEBUG_LOG( "SIMPLE CLICK");
//             }
//             m_latestTime = totalTimeElapsed;

        /*   
           if ( !m_2DViewer->getOldDrawer()->hasSelectedSet() ) //cas en que no hi ha cap conjunt de primitives seleccionat.
            {
                switch ( m_state )
                {
                    case STOPPED:
                        this->startROIAnnotation();
                    break;

                    case SIMULATING_ANNOTATION:
//                         if ( m_ROIType != POLYLINE )
                            this->stopROIAnnotation();
//                         else
//                             this->annotateNextPolylinePoint();
                    break;
                }
            }*/
        break;

        case vtkCommand::MouseMoveEvent:
            if( m_mainPolyline && ( m_mainPolyline->getNumberOfPoints() >= 2 ) )
            {
                this->simulateClosingPolyline();
                m_2DViewer->getDrawer()->refresh();
            }
        break;
    }
}

void PolylineROITool::annotateNewPoint()
{
    if (!m_mainPolyline )
    {
        m_mainPolyline = new DrawerPolyline;
        m_2DViewer->getDrawer()->draw( m_mainPolyline , Q2DViewer::Axial, m_2DViewer->getCurrentSlice() );
    }
    
    int xy[2];
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    m_2DViewer->getInteractor()->GetEventPosition( xy );
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim el punt
    m_mainPolyline->addPoint( computed );
    
//actualitzem els atributs de la polilinia
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );
}

void PolylineROITool::simulateClosingPolyline()
{
    if (!m_closingPolyline )
    {
        m_closingPolyline = new DrawerPolyline;
        m_closingPolyline->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
        m_2DViewer->getDrawer()->draw( m_closingPolyline , Q2DViewer::Axial, m_2DViewer->getCurrentSlice() );
    }
    
    m_closingPolyline->deleteAllPoints();

    int xy[2];
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    m_2DViewer->getInteractor()->GetEventPosition( xy );
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim els punts que simulen aquesta polilinia
    m_closingPolyline->addPoint( m_mainPolyline->getPoint( 0 ) );
    m_closingPolyline->addPoint( computed );
    m_closingPolyline->addPoint( m_mainPolyline->getPoint( m_mainPolyline->getNumberOfPoints() - 1 ) );
    
//actualitzem els atributs de la polilinia
    m_closingPolyline->update( DrawerPrimitive::VTKRepresentation );
}

}
