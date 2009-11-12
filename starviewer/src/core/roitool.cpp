/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "roitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolyline.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
//Qt

namespace udg {

ROITool::ROITool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "ROITool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );
    }

    m_closingPolyline = NULL;
    m_mainPolyline = NULL;
}

ROITool::~ROITool()
{
    if ( !m_mainPolyline.isNull() )
        delete m_mainPolyline;

    if ( !m_closingPolyline.isNull() )
        delete m_closingPolyline;
}

void ROITool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            if( m_2DViewer->getInput() )
            {
                switch( m_2DViewer->getInteractor()->GetRepeatCount() )
                {
                case 0: // Single-click o primer click d'un doble click. Afegim un nou punt a la ROI
                    annotateNewPoint();
                    m_2DViewer->getDrawer()->refresh();
                    break;

                case 1: // Doble-click, si tenim més de 2 punts, llavors tanquem la ROI
                    if( m_mainPolyline->getNumberOfPoints() > 2 )
                        closeForm();
                    break;
                }
            }
        break;

        case vtkCommand::MouseMoveEvent:

            if( m_mainPolyline && ( m_mainPolyline->getNumberOfPoints() >= 1 ) )
            {
                this->simulateClosingPolyline();
                m_2DViewer->getDrawer()->refresh();
            }
        break;
    }
}

void ROITool::annotateNewPoint()
{
    if (!m_mainPolyline )
    {
        m_mainPolyline = new DrawerPolyline;
        m_2DViewer->getDrawer()->draw( m_mainPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }

    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);
    
    //afegim el punt
    m_mainPolyline->addPoint( pickedPoint );

    //actualitzem els atributs de la polilinia
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

}

void ROITool::simulateClosingPolyline( )
{
    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);

    if (!m_closingPolyline )
    {
        m_closingPolyline = new DrawerPolyline;
        m_closingPolyline->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
        m_2DViewer->getDrawer()->draw( m_closingPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

        // Afegim els punts que simulen aquesta polilinia
        m_closingPolyline->addPoint( m_mainPolyline->getPoint( 0 ) );
        m_closingPolyline->addPoint( pickedPoint );
        m_closingPolyline->addPoint( m_mainPolyline->getPoint( m_mainPolyline->getNumberOfPoints() - 1 ) );
    }
    else
    {
        // Modifiquem els punts que han canviat
        m_closingPolyline->setPoint(1,pickedPoint);
        m_closingPolyline->setPoint(2,m_mainPolyline->getPoint( m_mainPolyline->getNumberOfPoints() - 1 ) );
    }

    //actualitzem els atributs de la polilinia
    m_closingPolyline->update( DrawerPrimitive::VTKRepresentation );
}

Volume::VoxelType ROITool::getGrayValue( double *coords )
{
    double *origin = m_2DViewer->getInput()->getOrigin();
    double *spacing = m_2DViewer->getInput()->getSpacing();
    int index[3];

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            index[0] = (int)((coords[0] - origin[0])/spacing[0]);
            index[1] = (int)((coords[1] - origin[1])/spacing[1]);
            index[2] = m_2DViewer->getCurrentSlice();
            break;

        case Q2DViewer::Sagital:
            index[0] = m_2DViewer->getCurrentSlice();
            index[1] = (int)((coords[1] - origin[1])/spacing[1]);
            index[2] = (int)((coords[2] - origin[2])/spacing[2]);
            break;

        case Q2DViewer::Coronal:
            index[0] = (int)((coords[0] - origin[0])/spacing[0]);
            index[1] = m_2DViewer->getCurrentSlice();
            index[2] = (int)((coords[2] - origin[2])/spacing[2]);
            break;
    }

    if ( m_2DViewer->isThickSlabActive() )
        return *((Volume::VoxelType *)m_2DViewer->getCurrentSlabProjection()->GetScalarPointer(index));
    else
        return *(m_2DViewer->getInput()->getScalarPointer(index));
}

void ROITool::closeForm()
{
    m_mainPolyline->addPoint( m_mainPolyline->getPoint( 0 ) );
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

    delete m_closingPolyline;

    m_2DViewer->getDrawer()->refresh();

    emit finished();

    m_mainPolyline=NULL;
}

}
