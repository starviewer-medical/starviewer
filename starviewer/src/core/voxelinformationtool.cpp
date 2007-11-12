/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "voxelinformationtool.h"

#include "q2dviewer.h"
#include "volume.h"

//vtk
#include <vtkCaptionActor2D.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

//Qt
#include <QDesktopWidget>
#include <QApplication>
#include <QPoint>

namespace udg {

VoxelInformationTool::VoxelInformationTool( Q2DViewer *viewer, QObject *parent )
 : /*Tool(parent),*/ m_isEnabled(false), m_voxelInformationCaption(0)
{
    m_2DViewer = viewer;
    createCaptionActor();
    
    connect( m_2DViewer, SIGNAL( sliceChanged(int) ), this, SLOT( isNeededUpdateVoxelInformation() ) );
}

VoxelInformationTool::~VoxelInformationTool()
{
}

void VoxelInformationTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::MouseMoveEvent:
        if( m_isEnabled )
            updateVoxelInformation();
    break;

    case vtkCommand::EnterEvent:
    break;

    case vtkCommand::LeaveEvent:
        m_voxelInformationCaption->VisibilityOff();
        m_2DViewer->refresh();
    break;

    default:
    break;
    }

}

void VoxelInformationTool::enable( bool enable )
{
    m_isEnabled = enable;
    if(!enable)
        m_voxelInformationCaption->VisibilityOff();
}

void VoxelInformationTool::createCaptionActor()
{
    double xyz[3];
    m_2DViewer->getCurrentCursorPosition(xyz);

    m_voxelInformationCaption = vtkCaptionActor2D::New();
    m_voxelInformationCaption->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->SetAttachmentPoint( xyz );
    m_voxelInformationCaption->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->BorderOff();
    m_voxelInformationCaption->LeaderOff();
    m_voxelInformationCaption->ThreeDimensionalLeaderOff();
    m_voxelInformationCaption->GetProperty()->SetColor( 1.0 , 0 , 0 );
    m_voxelInformationCaption->SetPadding( 0 );
    m_voxelInformationCaption->SetPosition( -1.0 , -1.0 );
    m_voxelInformationCaption->SetHeight( 0.05 );
    m_voxelInformationCaption->SetWidth( 0.3 );
    // propietats del texte
    m_voxelInformationCaption->GetCaptionTextProperty()->SetColor( 1. , 0.7 , 0.0 );
    m_voxelInformationCaption->GetCaptionTextProperty()->ShadowOn();
    m_voxelInformationCaption->GetCaptionTextProperty()->ItalicOff();
    m_voxelInformationCaption->GetCaptionTextProperty()->BoldOff();

    // l'afegim al Q2DViewer TODO ara només es té en compte 1 sol renderer!
    m_2DViewer->getRenderer()->AddActor( m_voxelInformationCaption );
}

void VoxelInformationTool::updateVoxelInformation()
{
    double xyz[3];
    
    double wPoint[4];
    int position[2];

    correctPositionOfCaption( position );

    QViewer::computeDisplayToWorld( m_2DViewer->getRenderer() , position[0] , position[1] , 0. , wPoint );
    xyz[0] = wPoint[0];
    xyz[1] = wPoint[1];
    depthAccordingViewAndSlice( xyz );
    placeText( xyz );
    
    m_2DViewer->refresh();
}

void VoxelInformationTool::depthAccordingViewAndSlice( double xyz[3] )
{
    int slice = m_2DViewer->getCurrentSlice();
    double *spacing = m_2DViewer->getInput()->getSpacing();
    double *origin = m_2DViewer->getInput()->getOrigin();
    
    //codi que soluciona el bug de les coordenades del voxel information (BUG: 122)
    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            xyz[2] = origin[2] + (slice * spacing[2]);
        break;
        case Q2DViewer::Sagittal:
            xyz[0] = origin[0] + (slice * spacing[0]);
        break;
        case Q2DViewer::Coronal:
            xyz[1] = origin[1] + (slice * spacing[1]);
        break;
    }
    
}

void VoxelInformationTool::placeText( double textPosition[3] )
{
    m_voxelInformationCaption->VisibilityOn();
    m_voxelInformationCaption->SetAttachmentPoint( textPosition );
    m_voxelInformationCaption->SetCaption( qPrintable( QString("(%1,%2,%3):%4").arg(textPosition[0],0,'f',2).arg(textPosition[1],0,'f',2).arg(textPosition[2],0,'f',2).arg( m_2DViewer->getCurrentImageValue() ) ) );
}

int* VoxelInformationTool::viewportDimensions()
{
    return m_2DViewer->getRenderer()->GetSize();
}

bool VoxelInformationTool::captionExceedsViewportTopLimit()
{
    int *eventPosition = m_2DViewer->getInteractor()->GetEventPosition();
    int *dimensions = viewportDimensions();
    double captionHeigth = ((double)dimensions[1]*0.05)+5.;
    
    return ( eventPosition[1]+captionHeigth > dimensions[1] );
}

bool VoxelInformationTool::captionExceedsViewportRightLimit()
{
    int *eventPosition = m_2DViewer->getInteractor()->GetEventPosition();
    int *dimensions = viewportDimensions();
    double captionWidth = ((double)dimensions[0]*0.3)+1.;
    
    return ( eventPosition[0]+captionWidth > dimensions[0] );
}


bool VoxelInformationTool::captionExceedsViewportLimits()
{
    return ( captionExceedsViewportTopLimit() || captionExceedsViewportRightLimit() );
}

void VoxelInformationTool::correctPositionOfCaption( int correctPositionInViewPort[2] )
{
    double xSecurityRange = 20.;
    int *eventPosition = m_2DViewer->getInteractor()->GetEventPosition();
    int *dimensions = viewportDimensions();
    double captionWidth = ((double)dimensions[0]*0.3)+xSecurityRange;
    
    correctPositionInViewPort[0] = eventPosition[0];
    correctPositionInViewPort[1] = eventPosition[1];
    
    if ( captionExceedsViewportRightLimit() )
    {
        correctPositionInViewPort[0] = eventPosition[0] - ( eventPosition[0] + captionWidth - dimensions[0] );
    }
    
    if ( captionExceedsViewportTopLimit() )
    {
        correctPositionInViewPort[1] = eventPosition[1] - 35;
    }
}

void VoxelInformationTool::isNeededUpdateVoxelInformation()
{
    if ( m_isEnabled )
    {
        updateVoxelInformation();
    }
}

}
