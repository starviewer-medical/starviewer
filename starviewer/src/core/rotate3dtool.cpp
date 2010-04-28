/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rotate3dtool.h"
#include "logging.h"
#include "qviewer.h"
//vtk
#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

Rotate3DTool::Rotate3DTool( QViewer *viewer, QObject *parent ) : Tool(viewer,parent)
{
    m_toolName = "Rotate3DTool";
    m_state = None;
    m_interactorStyle = vtkInteractorStyle::SafeDownCast( viewer->getInteractor()->GetInteractorStyle() );
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

Rotate3DTool::~Rotate3DTool()
{
}

void Rotate3DTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::RightButtonPressEvent:
        this->startRotate3D();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doRotate3D();
    break;

    case vtkCommand::RightButtonReleaseEvent:
        this->endRotate3D();
    break;

    default:
    break;
    }
}

void Rotate3DTool::startRotate3D()
{
    Q_ASSERT( m_interactorStyle );
    
    if( m_viewer->getInteractor()->GetControlKey() )
    {
        m_state = Spinning;
        // TODO podria ser que volguéssim posar-li una icona diferent per quan fem SPIN
        m_viewer->setCursor( QCursor(QPixmap(":/images/rotate3d.png")) );
        m_interactorStyle->StartSpin();
    }
    else
    {
        m_state = Rotating;
        m_viewer->setCursor( QCursor(QPixmap(":/images/rotate3d.png")) );
        m_interactorStyle->StartRotate();
    }
}

void Rotate3DTool::doRotate3D()
{
    Q_ASSERT( m_interactorStyle );
    
    switch( m_state )
    {
    case Rotating:        
        m_interactorStyle->Rotate();
    break;
    
    case Spinning:
        m_interactorStyle->Spin();
    break;
    }
}

void Rotate3DTool::endRotate3D()
{
    Q_ASSERT( m_interactorStyle );
    
    m_viewer->setCursor( Qt::ArrowCursor );
    switch( m_state )
    {
    case Rotating:
        m_interactorStyle->EndRotate();
    break;

    case Spinning:
        m_interactorStyle->EndSpin();
    break;
    }
    m_state = None;
}

}
