/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "voxelinformationtool.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawertext.h"
#include "drawer.h"
//vtk
#include <vtkCommand.h>

namespace udg {

VoxelInformationTool::VoxelInformationTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent), m_caption(0)
{
    m_toolName = "VoxelInformationTool";

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
    {
        DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );
    }
    createCaption();
    connect( m_2DViewer, SIGNAL( sliceChanged(int) ), SLOT( updateVoxelInformation() ) );
    connect( m_2DViewer, SIGNAL( phaseChanged(int) ), SLOT( updateVoxelInformation() ) );
    connect( m_2DViewer, SIGNAL( volumeChanged(Volume *) ), SLOT( inputChanged(Volume *) ) );
}

VoxelInformationTool::~VoxelInformationTool()
{
    delete m_caption;
}

void VoxelInformationTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::MouseMoveEvent:
        updateVoxelInformation();
    break;

    case vtkCommand::EnterEvent:
    break;

    case vtkCommand::LeaveEvent:
        m_caption->visibilityOff();
        m_caption->update(DrawerPrimitive::VTKRepresentation);
        m_2DViewer->refresh();
    break;

    default:
    break;
    }
}

void VoxelInformationTool::createCaption()
{
    m_caption = new DrawerText;
    m_caption->shadowOn();
    m_2DViewer->getDrawer()->draw( m_caption, Q2DViewer::Top2DPlane );
}

void VoxelInformationTool::updateVoxelInformation()
{
    if( !m_caption )
        return;
    
    double xyz[3];
    if( m_2DViewer->getCurrentCursorImageCoordinate(xyz) )
    {
        placeText(xyz);
    }
    else
    {
        m_caption->visibilityOff();
        m_caption->update(DrawerPrimitive::VTKRepresentation);
    }
    m_2DViewer->refresh();
}

void VoxelInformationTool::inputChanged(Volume *volume)
{
    createCaption();
}

void VoxelInformationTool::placeText( double textPosition[3] )
{
    if( !m_caption )
        return;
    
    double worldPoint[4];
    int position[2];
    double xyz[3];

    for ( int i = 0; i <3; i++)
        xyz[i] = textPosition[i];

    correctPositionOfCaption( position );
    m_2DViewer->computeDisplayToWorld( position[0] , position[1] , 0. , worldPoint );
    xyz[0] = worldPoint[0];
    xyz[1] = worldPoint[1];
    xyz[2] = worldPoint[2];

    Volume::VoxelType voxelValue;
    if( m_2DViewer->getCurrentCursorImageVoxel(voxelValue) )
    {
        m_caption->visibilityOn();
        m_caption->setAttachmentPoint(xyz);
        m_caption->setText( QString("(%1,%2,%3):%4").arg(textPosition[0],0,'f',2).arg(textPosition[1],0,'f',2).arg(textPosition[2],0,'f',2).arg( voxelValue ) );
        m_caption->update(DrawerPrimitive::VTKRepresentation);
    }
    else
        DEBUG_LOG("No s'ha trobat valor de la imatge");
}

bool VoxelInformationTool::captionExceedsViewportTopLimit()
{
    int *dimensions = m_2DViewer->getRenderWindowSize();
    double captionHeigth = ((double)dimensions[1]*0.05);

    return ( m_2DViewer->getEventPositionY()+captionHeigth > dimensions[1] );
}

bool VoxelInformationTool::captionExceedsViewportRightLimit()
{
    int *dimensions = m_2DViewer->getRenderWindowSize();
    double captionWidth = ((double)dimensions[0]*0.3)+1.;

    return ( m_2DViewer->getEventPositionX()+captionWidth > dimensions[0] );
}

bool VoxelInformationTool::captionExceedsViewportLimits()
{
    return ( captionExceedsViewportTopLimit() || captionExceedsViewportRightLimit() );
}

void VoxelInformationTool::correctPositionOfCaption( int correctPositionInViewPort[2] )
{
    double xSecurityRange = 20.;
    int eventPosition[2];
    m_2DViewer->getEventPosition( eventPosition );
    
    int *dimensions = m_2DViewer->getRenderWindowSize();
    double captionWidth = ((double)dimensions[0]*0.3)+xSecurityRange;
    double captionHeight = ((double)dimensions[1]*0.05)+xSecurityRange;

    correctPositionInViewPort[0] = eventPosition[0];
    correctPositionInViewPort[1] = eventPosition[1];

    if ( captionExceedsViewportRightLimit() )
    {
        correctPositionInViewPort[0] = eventPosition[0] - ( eventPosition[0] + captionWidth - dimensions[0] );
    }

    if ( captionExceedsViewportTopLimit() )
    {
        correctPositionInViewPort[1] = eventPosition[1] - ( eventPosition[1] + captionHeight - dimensions[1] );
    }
}

}
