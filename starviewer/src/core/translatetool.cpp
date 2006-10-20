/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "translatetool.h"
#include "zoomtool.h"
#include "q2dviewer.h"
#include "q3dviewer.h"
#include "q3dmprviewer.h"
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyle.h>
#include <vtkCommand.h>

#include "logging.h"

namespace udg {

TranslateTool::TranslateTool( Q2DViewer *viewer , QObject *parent, const char *name )
//  : Tool(parent)
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
}

TranslateTool::TranslateTool( Q3DViewer *viewer , QObject *parent, const char *name )
//  : Tool(parent)
{
    m_state = NONE;
    m_interactorStyle = viewer->getInteractorStyle();
}

TranslateTool::TranslateTool( Q3DMPRViewer *viewer , QObject *parent, const char *name )
//  : Tool(parent)
{
    m_state = NONE;
    // \TODO implement me
//     m_interactorStyle = viewer->getInteractorStyle();
}

TranslateTool::~TranslateTool()
{
}

void TranslateTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        this->startTranslate();
    break;

    case vtkCommand::MouseMoveEvent:
        this->doTranslate();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        this->endTranslate();
    break;
    
    default:
    break;
    }
}

void TranslateTool::startTranslate()
{
    m_state = TRANSLATING;
    m_interactorStyle->StartPan();
}

void TranslateTool::doTranslate()
{
    if( m_state == TRANSLATING )
    {
        m_interactorStyle->Pan();
    }
}

void TranslateTool::endTranslate()
{
    m_state = NONE;
    m_interactorStyle->EndPan();
}

}
