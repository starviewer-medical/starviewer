/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "oldrotate3dtool.h"
#include "logging.h"
#include "q3dviewer.h"
#include "q3dmprviewer.h"

#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyle.h>
#include <vtkCommand.h>

namespace udg {

OldRotate3DTool::OldRotate3DTool( Q3DViewer *viewer, QObject *parent )
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

OldRotate3DTool::OldRotate3DTool( Q3DMPRViewer *viewer, QObject *parent )
{
    m_state = NONE;
    // \TODO implement me
    m_interactorStyle = viewer->getInteractorStyle();
    if( !m_interactorStyle )
        DEBUG_LOG( "L'interactor Style és buit!" );
}

OldRotate3DTool::~OldRotate3DTool()
{
}

void OldRotate3DTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startRotate3D();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doRotate3D();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endRotate3D();
    break;

    default:
    break;
    }
}

void OldRotate3DTool::startRotate3D()
{
    if( m_interactorStyle )
    {
        m_state = ROTATING;
        m_interactorStyle->StartRotate();
    }
    else
        DEBUG_LOG( "::startRotate3D(): L'interactor Style és buit!" );
}

void OldRotate3DTool::doRotate3D()
{
    if( m_interactorStyle )
    {
        if( m_state == ROTATING )
            m_interactorStyle->Rotate();
    }
    else
        DEBUG_LOG( "::doRotate3D(): L'interactor Style és buit!" );

}

void OldRotate3DTool::endRotate3D()
{
    if( m_interactorStyle )
    {
        m_state = NONE;
        m_interactorStyle->EndRotate();
    }
    else
        DEBUG_LOG( "::endRotate3D(): L'interactor Style és buit!" );
}
}
