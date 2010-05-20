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
#include "drawerpolygon.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

ROITool::ROITool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent), m_roiPolygon(0), m_hasToComputeStatisticsData(true), m_mainPolyline(0), m_closingPolyline(0)
{
    m_toolName = "ROITool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );
    }

    connect( m_2DViewer, SIGNAL( volumeChanged(Volume *) ), SLOT( initialize() ) );
}

ROITool::~ROITool()
{
    bool hasToRefresh = false;
    // Cal decrementar el reference count perquè 
    // l'annotació s'esborri si "matem" l'eina
    if ( !m_mainPolyline.isNull() )
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
        hasToRefresh = true;
    }

    if ( !m_closingPolyline.isNull() )
    {
        m_closingPolyline->decreaseReferenceCount();
        delete m_closingPolyline;
        hasToRefresh = true;
    }

    if ( !m_roiPolygon.isNull() )
        delete m_roiPolygon;

    if( hasToRefresh )
        m_2DViewer->render();
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
                m_2DViewer->render();
            }
        break;
    }
}

void ROITool::annotateNewPoint()
{
    if (!m_mainPolyline )
    {
        m_mainPolyline = new DrawerPolyline;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_mainPolyline->increaseReferenceCount();
        m_2DViewer->getDrawer()->draw( m_mainPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
    if(!m_roiPolygon)
    {
        m_roiPolygon = new DrawerPolygon;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_roiPolygon->increaseReferenceCount();
    }

    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);
    
    // Afegim el punt al polígon de la ROI
    m_roiPolygon->addVertix( pickedPoint );
    // Afegim el punt de la polilínia que estem pintant
    m_mainPolyline->addPoint( pickedPoint );
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

    // Com que estem afegint punts cal indicar que si és necessari recalcular les dades estadístiques
    m_hasToComputeStatisticsData = true;
}

void ROITool::simulateClosingPolyline()
{
    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);

    if (!m_closingPolyline )
    {
        m_closingPolyline = new DrawerPolyline;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_closingPolyline->increaseReferenceCount();
        m_closingPolyline->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
        m_2DViewer->getDrawer()->draw( m_closingPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

        // Afegim els punts que simulen aquesta polilínia
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

    // Actualitzem els atributs de la polilínia
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
    // Així alliberem les primitives perquè puguin ser esborrades
    m_closingPolyline->decreaseReferenceCount();
    m_mainPolyline->decreaseReferenceCount();
    m_roiPolygon->decreaseReferenceCount();
    // Eliminem les polilínies amb les que hem simulat el dibuix de la ROI
    delete m_closingPolyline;
    delete m_mainPolyline;

    // Dibuixem el polígon resultant
    m_2DViewer->getDrawer()->draw( m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    m_roiPolygon->update( DrawerPrimitive::VTKRepresentation );

    // Indiquem que hem finalitzat les tasques de dibuix
    emit finished();

    // Un cop fets els càlculs, fem el punter nul per poder controlar si podem fer una nova roi o no
    // No fem delete, perquè sinó això faria que s'esborrés del drawer
    m_roiPolygon = NULL;
}

void ROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if ( !m_mainPolyline.isNull() )
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
    }

    if ( !m_closingPolyline.isNull() )
    {
        m_closingPolyline->decreaseReferenceCount();
        delete m_closingPolyline;
    }

    if ( !m_roiPolygon.isNull() )
        delete m_roiPolygon;
    
    m_closingPolyline = NULL;
    m_mainPolyline = NULL;
    m_roiPolygon = NULL;
}

}
